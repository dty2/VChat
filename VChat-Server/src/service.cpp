#include "service.h"
#include "connection.h"
#include "store.h"
#include "vchat.pb.h"
#include <functional>
#include <vector>

namespace vchat {

extern std::shared_ptr<RedisStore> redis;
extern std::shared_ptr<SqliteStore> sqlite;

int Service::generateId() {
  return 0;
}

void Service::boardcast(int id, const std::function<void(int)>& callback) {
  std::vector<Friend> friends;
  sqlite->getFriends(id, friends);
  for(auto v : friends)
    callback(v.id());
}

// ------------------------------------------------------------ sign
void Service::sign(RequestSign sign, const Connection_ptr& cp) {
  if (sign.has_signinfo()) signIn(sign, cp);
  else signOut(sign, cp);
}

// sign in
void Service::signIn(RequestSign sign, const Connection_ptr& cp) {
  int id = generateId();
  Basic basic = sign.signinfo().basic();
  basic.set_id(id);

  std::function<void()> callback = [=]{
    sqlite->insertBasic(basic);
    sqlite->insertPersonal(sign.signinfo().personal());
    redis->add(id);
  };

  ResponseSign res;
  res.set_id(id);

  Head head = Connection::enpackHead(
    PackageType::response,
    ServiceType::sign,
    res.SerializePartialAsString().size()
  );

  cp->write<ResponseSign>(head, res, callback);
}

// sign out
void Service::signOut(RequestSign sign, const Connection_ptr& cp) {
  int id = sign.id();

  sqlite->deleteBasic(id);
  sqlite->deletePersonal(id);
  sqlite->deleteFriend(id);
  sqlite->deleteMsg(id);
  redis->remove(id);

  NotifySign res;
  res.set_id(id);

  Head head = Connection::enpackHead(
    PackageType::notify,
    ServiceType::sign,
    res.SerializePartialAsString().size()
  );

  boardcast(id, [=](int id_) {
    if(redis->checkOnline(id_))
      routingtable[id_]->write<NotifySign>(head, res);
    else
      redis->insertSign(id_, id);
  });
}

// ------------------------------------------------------------ log
void Service::getOffline(OfflineInfo& offlineinfo, int id) {
  std::vector<Basic> self;
  redis->getChange(id, self);
  for(auto v : self)
    *offlineinfo.add_friends() = v;

  std::vector<Msg> msg;
  redis->getMsg(id, msg);
  for(auto v : msg)
    *offlineinfo.add_msgs() = v;

  std::vector<Basic> notifymanage;
  redis->getManage(id, notifymanage);
  for(auto v : notifymanage)
    *offlineinfo.add_notifymanage() = v;

  std::vector<int> responsemanage;
  redis->getManage(id, responsemanage);
  for(auto v : responsemanage)
    offlineinfo.add_responsemanage(id);
}

void Service::log(RequestLog log) {
  if (log.has_loginfo()) logIn(log);
  else logOut(log);
}

// log in
void Service::logIn(RequestLog log) {
  int id = log.loginfo().id();

  // log error handle
  ResponseLog res;
  if(!redis->checkOnline(id)) {
    res.set_reason(LogReason::repeat);
    return;
  }

  if(!sqlite->checkPassword(id, log.loginfo().password())) {
    res.set_reason(LogReason::wrong);
    return;
  }

  if(!sqlite->checkId(id)) {
    res.set_reason(LogReason::idnotexist);
    return;
  }

  // success
  // get msg info from offline library
  OfflineInfo offlineinfo;
  getOffline(offlineinfo, id);
  res.set_allocated_offlineinfo(&offlineinfo);

  Head head = Connection::enpackHead(
    PackageType::response,
    ServiceType::log,
    res.SerializePartialAsString().size()
  );

  std::function<void()> callback = [=] {
    redis->setOnline(id, true);
    NotifyLog notify;
    notify.set_status(true);
    notify.set_id(id);
    Head notifyhead = Connection::enpackHead(
      PackageType::notify,
      ServiceType::log,
      notify.SerializePartialAsString().size()
    );
    boardcast(id, [=](int id_){
      if(redis->checkOnline(id_))
        routingtable[id_]->write<NotifyLog>(notifyhead, notify);
    });

    // delete offline info in redis
    redis->deleteChange(id);
    redis->deleteMsg(id);
    redis->deleteManage(id);
  };

  routingtable[id]->write<ResponseLog>(head, res, callback);
}

// log out
void Service::logOut(RequestLog log) {
  int id = log.id();
  redis->setOnline(id, false);

  NotifyLog notify;
  notify.set_status(false);
  notify.set_id(id);
  Head notifyhead = Connection::enpackHead(
    PackageType::notify,
    ServiceType::log,
    notify.SerializePartialAsString().size()
  );

  boardcast(id, [=](int id_){
    if(redis->checkOnline(id_))
      routingtable[id_]->write<NotifyLog>(notifyhead, notify);
  });
}

// ------------------------------------------------------------ Synchronous
void Service::synchronous(RequestSynchronous sync) {
  int id = sync.id();

  ResponseSynchronous res;

  Basic basic;
  sqlite->getBasic(id, basic);
  res.set_allocated_basic(&basic);

  Personal personal;
  sqlite->getPersonal(id, personal);
  res.set_allocated_personal(&personal);

  std::vector<Friend> friends;
  sqlite->getFriends(id, friends);
  for(auto v : friends)
    *res.add_friends() = v;

  std::vector<Msg> msgs;
  sqlite->getMsg(id, msgs);
  for(auto v : msgs)
    *res.add_msgs() = v;

  Head head = Connection::enpackHead(
    PackageType::response,
    ServiceType::synchronous,
    res.SerializePartialAsString().size()
  );

  routingtable[id]->write<ResponseSynchronous>(head, res);
}

// ------------------------------------------------------------ change
void Service::change(RequestChange change) {
  if (change.has_basic()) changeBasic(change);
  else if (change.has_friend_()) changePersonal(change);
  else changeFriend(change);
}

void Service::changeBasic(RequestChange change) {
  int id = change.basic().id();
  Basic basic = change.basic();

  sqlite->changeBasic(basic);

  NotifyChange res;
  res.set_allocated_basic(&basic);

  Head head = Connection::enpackHead(
    PackageType::notify,
    ServiceType::change,
    res.SerializePartialAsString().size()
  );

  boardcast(id, [=](int id_) {
    if(redis->checkOnline(id_))
      routingtable[id_]->write<NotifyChange>(head, res);
    else
      redis->insertChange(id_, change.basic());
  });
}

void Service::changePersonal(RequestChange change) {
  sqlite->changePersonal(change.personal());
}

void changeFriend(RequestChange change) {
  sqlite->changeFriend(change.friend_());
}

// ------------------------------------------------------------ find
void Service::find(RequestFind find, const Connection_ptr& cp) {
  std::vector<Basic> basic;
  if(find.has_id())
    sqlite->getBasic(find.id(), basic.front());
  else
    sqlite->getBasic(find.name(), basic);

  ResponseFind res;
  for(auto v : basic)
    *res.add_basics() = v;

  Head head = Connection::enpackHead(
    PackageType::response,
    ServiceType::find,
    res.SerializePartialAsString().size()
  );

  cp->write<ResponseFind>(head, res);
}

// ------------------------------------------------------------ manage
void Service::manage(RequestManage manage) {
  if (manage.op()) addFriend(manage);
  else deleteFriend(manage);
}

void Service::addFriend(RequestManage manage) {
  Basic basic;
  if(manage.initiative()) {
    sqlite->getBasic(manage.id(), basic);
    redis->insertAdd(manage.id(), manage.friendid());

    if(!redis->checkOnline(manage.friendid())) {
      // true for add, false for delete
      redis->insertManage(true, true, basic);
      return;
    }

    NotifyManage res;
    res.set_initiative( true);
    res.set_op(true);
    res.set_allocated_basic(&basic);

    Head head = Connection::enpackHead(
      PackageType::notify,
      ServiceType::manage,
      res.SerializePartialAsString().size()
    );
    routingtable[manage.friendid()]->write<NotifyManage>(head, res);

  } else {
    Friend friend_;
    friend_.set_id(manage.friendid());
    friend_.set_friendid(manage.id());
    sqlite->insertFriend(manage.friendid(), friend_);

    sqlite->getBasic(manage.id(), basic);

    if(!redis->checkOnline(manage.friendid())) {
      // true for add, false for delete
      redis->insertManage(false, true, basic);
      return;
    }

    NotifyManage res;
    res.set_initiative(false);
    res.set_op(true);
    res.set_allocated_basic(&basic);

    Head head = Connection::enpackHead(
      PackageType::notify,
      ServiceType::manage,
      res.SerializePartialAsString().size()
    );
    routingtable[manage.friendid()]->write<NotifyManage>(head, res);
  }
}

void Service::deleteFriend(RequestManage manage) {
  Basic basic;
  if(manage.initiative()) {
    basic.set_id(manage.id());
    sqlite->deleteFriend(manage.id(), manage.friendid());
    sqlite->deleteMsg(manage.id(), manage.friendid());

    if(!redis->checkOnline(manage.friendid())) {
      // true for add, false for delete
      redis->insertManage(true, false, basic);
      return;
    }

    NotifyManage res;
    res.set_initiative(true);
    res.set_op(false);
    res.set_allocated_basic(&basic);

    Head head = Connection::enpackHead(
      PackageType::notify,
      ServiceType::manage,
      res.SerializePartialAsString().size()
    );
    routingtable[manage.friendid()]->write<NotifyManage>(head, res);

  } else {
    sqlite->getBasic(manage.id(), basic);
    if(!redis->checkOnline(manage.friendid())) {
      // true for add, false for delete
      redis->insertManage(false, false, basic);
      return;
    }

    NotifyManage res;
    res.set_initiative(false);
    res.set_op(false);
    res.set_allocated_basic(&basic);

    Head head = Connection::enpackHead(
      PackageType::notify,
      ServiceType::manage,
      res.SerializePartialAsString().size()
    );
    routingtable[manage.friendid()]->write<NotifyManage>(head, res);
  }
}

// ------------------------------------------------------------ msg
void Service::msg(RequestMsg msg) {
  Msg msg_ = msg.msg();
  sqlite->insertMsg(msg_);
  if(!redis->checkOnline(msg_.receiver())) {
    redis->insertMsg(msg_);
    return;
  }

  NotifyMsg res;
  res.set_allocated_msg(&msg_);
  Head head = Connection::enpackHead(
    PackageType::notify,
    ServiceType::msg,
    res.SerializePartialAsString().size()
  );
  routingtable[msg.msg().receiver()]->write<NotifyMsg>(head, res);
}

} // namespace vchat
