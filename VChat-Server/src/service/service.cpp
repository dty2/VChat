#include "service.h"
#include "connection.h"
#include "package.h"

Service *Service::service = nullptr;
Service::Cache Service::cache;

Service::Service() {}

void Service::getinstance() { service = new Service(); }

void Service::serve(int method, Json::Value value, Connection *connection) {
  switch (method) {
  case method::signin:
    service->signin(value, connection);
    break;
  // case method::signout: service->signout(value, connection); break;
  case method::login:
    service->login(value, connection);
    break;
  // case method::logout: service->logout(value, connection); break;
  case method::findfd:
    service->find(value, connection);
    break;
  case method::addfd:
    service->addfriend(value, connection);
    break;
  case method::accept_addfd:
    service->addfriend(method, value, connection);
    break;
  case method::refuse_addfd:
    service->addfriend(method, value, connection);
    break;
  // case method::cdeletefd: service->deletefriend(value, connection); break;
  case method::sendmsg:
    service->sendmsg(value);
    break;
  }
}

void Service::signin(Json::Value value, Connection *connection) {
  PersionalInfo persionalinfo;
  try {
    persionalinfo.id = value["id"].asInt();
    persionalinfo.password = value["password"].asInt();
    persionalinfo.username = value["username"].asString();
  } catch (const std::exception &e) {
    LOG(INFO) << persionalinfo.id << "sign error" << " because format error";
    connection->write(method::signin_fmerr, 0);
  }
  if (Store::store->insertPersional(persionalinfo)) {
    LOG(INFO) << persionalinfo.id << "sign successful";
    connection->write(method::signin_suc, 0);
  } else {
    LOG(INFO) << persionalinfo.id << " sign error" << " because id exist";
    connection->write(method::signin_idexist, 0);
  }
}

void Service::login(Json::Value value, Connection *connection) {
  try {
    int id = value["id"].asInt();
    int password = value["password"].asInt();
    PersionalInfo persionalinfo;
    if (!Store::store->getPersional(persionalinfo, id)) {
      LOG(INFO) << id << " log error" << " because no id";
      connection->write(method::login_noid, value);
      return;
    }
    if (persionalinfo.password == password) {
      UserInfo userinfo;
      Store::store->getUser(userinfo, id);
      Json::Value root, persionalinfo, friendlist, messagelist;
      persionalinfo["id"] = userinfo.persionalinfo.id;
      persionalinfo["password"] = userinfo.persionalinfo.password;
      persionalinfo["username"] = userinfo.persionalinfo.username;
      for (auto x : userinfo.friendlist) {
        Json::Value friendinfo;
        friendinfo["friendid"] = x.friendid;
        friendinfo["friendname"] = x.friendname;
        friendlist.append(friendinfo);
      }
      for (auto x : userinfo.messagelist) {
        Json::Value messageinfo;
        messageinfo["sender"] = x.sender;
        messageinfo["receiver"] = x.receiver;
        messageinfo["message"] = x.msg;
        messageinfo["time"] = x.time;
        messagelist.append(messageinfo);
      }
      root["persionalinfo"] = persionalinfo;
      root["friendlist"] = friendlist;
      root["messagelist"] = messagelist;
      Service::cache.online[id] = connection;
      LOG(INFO) << id << " log in";
      connection->write(method::login_suc, root);
      for (auto x : this->cache.addfriendapply[id])
        connection->write(method::addfd, x);
    } else {
      LOG(INFO) << id << " log error" << " because password error";
      connection->write(method::login_pwerr, value);
    }
  } catch (const std::exception &e) {
    connection->write(method::login_err, 0);
  }
}

void Service::sendmsg(Json::Value value) {
  try {
    MessageInfo messageinfo;
    messageinfo.sender = value["sender"].asInt();
    messageinfo.receiver = value["receiver"].asInt();
    messageinfo.msg = value["message"].asString();
    messageinfo.time = value["time"].asInt64();
    bool op = Store::store->insertMessage(messageinfo);
    if (Service::cache.online[messageinfo.receiver]) {
      Service::cache.online[messageinfo.receiver]->write(method::sendmsg,
                                                         value);
    }
  } catch (const std::exception &e) {
    LOG(INFO) << e.what();
  }
}

void Service::find(Json::Value value, Connection *connection) {
  try {
    int type = value["method"].asInt();
    int id = value["id"].asInt();
    LOG(INFO) << "type is" << type << id << "find";
    if (!type) {
      PersionalInfo persionalinfo;
      int findid = value["findid"].asInt();
      LOG(INFO) << "findid :" << findid;
      if (Store::store->getPersional(persionalinfo, findid)) {
        Json::Value root, findlist, findinfo;
        findinfo["id"] = persionalinfo.id;
        findinfo["name"] = persionalinfo.username;
        findlist.append(findinfo);
        root["findlist"] = findlist;
        LOG(INFO) << id << "find" << persionalinfo.id << " "
                  << persionalinfo.username;
        if (Service::cache.online[id])
          Service::cache.online[id]->write(method::findfd_suc, root);
      } else {
        if (Service::cache.online[id])
          Service::cache.online[id]->write(method::findfd_err, 0);
      }
    } else {
      std::vector<PersionalInfo> persionalinfolist;
      std::string name = value["name"].asString();
      if (Store::store->getPersional(persionalinfolist, name)) {
        Json::Value root, findlist;
        for (auto &v : persionalinfolist) {
          Json::Value findinfo;
          findinfo["id"] = v.id;
          findinfo["name"] = v.username;
          findinfo.append(findinfo);
        }
        root["findlist"] = findlist;
        if (Service::cache.online[id])
          Service::cache.online[id]->write(method::findfd_suc, root);
      } else {
        if (Service::cache.online[id])
          Service::cache.online[id]->write(method::findfd_err, 0);
      }
    }
  } catch (const std::exception &e) {
    LOG(INFO) << e.what();
  }
}

void Service::addfriend(Json::Value value, Connection *connection) {
  try {
    int friendid = value["friendid"].asInt();
    if (Service::cache.online[friendid])
      Service::cache.online[friendid]->write(method::addfd, value);
    else
      Service::cache.addfriendapply[friendid].emplace_back(value);
  } catch (const std::exception &e) {
    LOG(INFO) << e.what();
  }
}

void Service::addfriend(int method, Json::Value value, Connection *connection) {
  try {
    FriendInfo friendinfo;
    int userid = value["friendid"].asInt();        // 添加好友的人
    friendinfo.friendid = value["userid"].asInt(); // 被添加的人
    friendinfo.friendname = value["username"].asString();
    if (method == method::accept_addfd) {
      bool op = Store::store->insertFriend(friendinfo, userid);
      userid = friendinfo.friendid;
      PersionalInfo temp;
      Store::store->getPersional(temp, value["friendid"].asInt());
      friendinfo.friendid = temp.id;
      friendinfo.friendname = temp.username;
      bool opp = Store::store->insertFriend(friendinfo, userid);
      if (Service::cache.online[friendinfo.friendid])
        Service::cache.online[friendinfo.friendid]->write(method::accept_addfd,
                                                          value);
    } else {
      if (Service::cache.online[friendinfo.friendid])
        Service::cache.online[friendinfo.friendid]->write(method::refuse_addfd,
                                                          value);
    }
  } catch (const std::exception &e) {
    LOG(INFO) << e.what();
  }
}
