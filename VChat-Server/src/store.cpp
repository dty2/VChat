#include "store.h"
#include "connection.h"
#include <cstdlib>
#include <iterator>
#include <unordered_map>
#include <vector>

namespace vchat {
SqliteStore::SqliteStore(std::string db_address) {
  db = std::make_unique<SQLite::Database>(
    db_address, 
    SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE
  );
}

bool SqliteStore::checkId(int id) {
  try {
    SQLite::Statement query(
      *db,
      "SELECT EXISTS(SELECT 1 FROM basic WHERE id = ?)"
      );
    query.bind(1, id);
    while(query.executeStep()) {
      return query.getColumn(0).getInt();
    }
    return false;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::checkPassword(int id, int password) {
  try {
    SQLite::Statement query(
      *db,
  "SELECT password FROM personal WHERE id = ?"
      );
    query.bind(1, id);
    while(query.executeStep()) {
      return password == query.getColumn(0).getInt();
    }
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::getBasic(int id, Basic& basic) {
  try {
    SQLite::Statement query(
      *db,
      "SELECT * FROM basic WHERE id = ?"
      );
    query.bind(1, id);
    while(query.executeStep()) {
      basic.set_id(query.getColumn(0).getInt());
      basic.set_name(query.getColumn(1).getString());
      basic.set_gender(query.getColumn(2).getInt());
      basic.set_age(query.getColumn(3).getInt());
      basic.set_signature(query.getColumn(4).getString());
    }
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error:" << e.what();
    return false;
  }
}

bool SqliteStore::getBasic(std::string name, std::vector<Basic>& basic) {
  try {
    SQLite::Statement query(
      *db,
      "SELECT * FROM personal WHERE name_ = ?"
    );
    query.bind(1, name);
    for(auto& v : basic)
      while(query.executeStep()) {
        v.set_id(query.getColumn(0).getInt());
        v.set_name(query.getColumn(1).getString());
        v.set_gender(query.getColumn(2).getInt());
        v.set_age(query.getColumn(3).getInt());
        v.set_signature(query.getColumn(4).getString());
      }
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::insertBasic(const Basic& basic) {
  try {
    SQLite::Statement query(
      *db,
      "INSERT INTO basic VALUES (?, ?, ?, ?, ?)"
    );
    query.bind(1, basic.id());
    query.bind(2, basic.name());
    query.bind(3, basic.gender());
    query.bind(4, basic.age());
    query.bind(5, basic.signature());
    query.exec();
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::changeBasic(const Basic& basic) {
  try {
    SQLite::Statement query(
      *db,
      "UPDATE basic SET name_ = ?, gender = ?, age = ?, signature = ? WHERE id = ?"
    );
    query.bind(1, basic.name());
    query.bind(2, basic.gender());
    query.bind(3, basic.age());
    query.bind(4, basic.signature());
    query.bind(5, basic.id());
    query.exec();
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::deleteBasic(int id) {
  try {
    SQLite::Statement query(
      *db,
      "DELETE from basic WHERE id = ?"
    );
    query.bind(1, id);
    query.exec();
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::getPersonal(int id, Personal& info) {
  try {
    SQLite::Statement query(
      *db,
      "SELECT * from personal WHERE id = ?"
    );
    query.bind(1, id);
    while(query.executeStep()) {
      info.set_id(query.getColumn(0).getInt());
      info.set_password(query.getColumn(1).getInt());
    }
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::insertPersonal(const Personal& personal) {
  try {
    SQLite::Statement query(
      *db,
      "INSERT INTO personal VALUES (?, ?)"
    );
    query.bind(1, personal.id());
    query.bind(2, personal.password());
    query.exec();
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::changePersonal(const Personal& personal) {
  try {
    SQLite::Statement query(
      *db,
      "UPDATE basic SET password_ = ? WHERE id = ?"
    );
    query.bind(1, personal.password());
    query.bind(2, personal.id());
    query.exec();
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::deletePersonal(int id) {
  try {
    SQLite::Statement query(
      *db,
      "DELETE FROM personal WHERE id = ?"
    );
    query.bind(1, id);
    query.exec();
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::getFriends(int id, std::vector<Friend>& friends) {
  try {
    SQLite::Statement query(
      *db,
      "SELECT * FROM friend WHERR id = ?"
    );
    query.bind(1, id);
    for(auto& v : friends)
      while(query.executeStep()) {
        v.set_id(query.getColumn(0).getInt());
        v.set_friendid(query.getColumn(1).getInt());
        v.set_remark(query.getColumn(2).getString());
      }
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::insertFriend(int id, const Friend& friend_) {
  try {
    SQLite::Statement query(
      *db,
      "INSERT INTO friend VALUES (?, ?, ?)"
    );
    query.bind(1, friend_.id());
    query.bind(2, friend_.friendid());
    query.bind(3, friend_.remark());
    query.exec();
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::changeFriend(const Friend& friend_) {
  try {
    SQLite::Statement query(
      *db,
      "UPDATE friend SET remark = ? WHERE id = ?"
    );
    query.bind(1, friend_.remark());
    query.bind(2, friend_.id());
    query.exec();
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::deleteFriend(int id) {
  try {
    SQLite::Statement query(
      *db,
      "DELETE FROM friend WHERE id = ?"
    );
    query.bind(1, id);
    query.exec();
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}
bool SqliteStore::deleteFriend(int id, int friendid) {
  try {
    SQLite::Statement query(
      *db,
      "DELETE FROM friend WHERE id = ? AND friendid = ?"
    );
    query.bind(1, id);
    query.bind(2, friendid);
    query.exec();
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::getMsg(int id, std::vector<Msg>& msgs) {
  try {
    SQLite::Statement query(
      *db,
      "SELECT * FROM message WHERE id = ?"
    );
    query.bind(1, id);
    for(auto& v : msgs)
      while(query.executeStep()) {
        v.set_id(query.getColumn(0).getInt());
        v.set_sender(query.getColumn(1).getInt());
        v.set_receiver(query.getColumn(2).getInt());
        v.set_msg(query.getColumn(3).getString());
        v.set_time(query.getColumn(4).getInt());
      }
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::insertMsg(Msg& msg) {
  try {
    SQLite::Statement query(
      *db,
      "INSERT INTO message VALUES (?, ?, ?, ?, ?)"
    );
    query.bind(1, msg.id());
    query.bind(2, msg.sender());
    query.bind(3, msg.receiver());
    query.bind(4, msg.msg());
    query.bind(5, msg.time());
    query.exec();
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}
bool SqliteStore::deleteMsg(int id) {
  try {
    SQLite::Statement query(
      *db,
      "DELETE from message WHERE sender = ?"
    );
    query.bind(1, id);
    query.exec();
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

bool SqliteStore::deleteMsg(int sender, int receiver) {
  try {
    SQLite::Statement query(
      *db,
      "DELETE from message WHERE sender = ? AND receiver = ?"
    );
    query.bind(1, sender);
    query.bind(2, receiver);
    query.exec();
    return true;
  } catch (std::exception& e) {
    LOG(INFO) << "check sqlite error" << e.what();
    return false;
  }
}

RedisStore::RedisStore(std::string redis_port) {
  redis = std::make_unique<Redis>(redis_port);
}

bool RedisStore::add(int id) {
  try {
    redis->rpush("OfflineInfo:" + std::to_string(id) + ":sign", {});
    redis->rpush("OfflineInfo:" + std::to_string(id) + ":change", {});
    redis->rpush("OfflineInfo:" + std::to_string(id) + ":manage", {});
    redis->rpush("OfflineInfo:" + std::to_string(id) + ":msg", {});
    redis->set("KeepInfo:" + std::to_string(id) + ":online", {});
    redis->rpush("KeepInfo:" + std::to_string(id) + ":add", {});
    return true;
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
    return false;
  }
}

bool RedisStore::remove(int id) {
  try {
    return true;
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
    return false;
  }
}

// OfflineInfo:IDa:IDb (list)
// IDb is sign out and IDa is not online
bool RedisStore::insertSign(int id_, int id){
  try {
    redis->rpush(
      "OfflineInfo:" + std::to_string(id_) + ":sign",
      std::to_string(id)
    );
    return true;
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
    return false;
  }
}

// KeepInfo:ID:online (string)
bool RedisStore::checkOnline(int id){
  try {
    std::string online = *redis->get(
      "KeepInfo:" + std::to_string(id) + ":online");
    return online == "1";
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
    return false;
  }
}

// KeepInfo:ID:online (string)
bool RedisStore::setOnline(int id, bool online){
  try {
    redis->set(
      "KeepInfo:" + std::to_string(id) + ":online",
      std::to_string(online)
    );
    return true;
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
    return false;
  }
}

// OfflineInfo:ID:change (list as hash table head)
// ID:ID:change (hash table)
bool RedisStore::insertChange(int id, const Basic& basic){
  try {
    redis->rpush(
      "OfflineInfo:" + std::to_string(id) + ":change",
      std::to_string(basic.id()) + std::to_string(id) + "change"
    );
    redis->hset(
      std::to_string(basic.id()) + std::to_string(id) + "change",
      "id",
      std::to_string(basic.id())
    );
    redis->hset(
      std::to_string(basic.id()) + std::to_string(id) + "change",
      "name",
      basic.name()
    );
    redis->hset(
      std::to_string(basic.id()) + std::to_string(id) + "change",
      "gender",
      std::to_string(basic.gender())
    );
    redis->hset(
      std::to_string(basic.id()) + std::to_string(id) + "change",
      "age",
      std::to_string(basic.age())
    );
    redis->hset(
      std::to_string(basic.id()) + std::to_string(id) + "change",
      "signature",
      basic.signature()
    );
    return true;
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
    return false;
  }
}

bool RedisStore::getChange(int id, std::vector<Basic>& list){
  try {
    std::unordered_map<std::string, std::string> hash;
    std::vector<std::string> head;
    redis->lrange(
      "OfflineInfo:" + std::to_string(id) + ":change",
      0, -1, std::back_inserter(head)
    );
    for(int i = 0; i < head.size(); i ++) {
      redis->hgetall(
        head[i],
        std::inserter(hash, hash.begin())
      );
      list[i].set_id(std::atoi(hash["id"].c_str()));
      list[i].set_name(hash["name"]);
      list[i].set_gender(std::atoi(hash["gender"].c_str()));
      list[i].set_age(std::atoi(hash["age"].c_str()));
      list[i].set_signature(hash["id"]);
    }
    return true;
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
    return false;
  }
}

bool RedisStore::deleteChange(int id){
  try {
    std::unordered_map<std::string, std::string> hash;
    std::vector<std::string> head;
    redis->del(
      0, -1, std::back_inserter(head)
    );

    redis->lrem(
      "OfflineInfo:" + std::to_string(id) + ":change",
      0,
      "a");
    for(int i = 0; i < head.size(); i ++) {
      redis->hgetall(
        head[i],
        std::inserter(hash, hash.begin())
      );
      list[i].set_id(std::atoi(hash["id"].c_str()));
      list[i].set_name(hash["name"]);
      list[i].set_gender(std::atoi(hash["gender"].c_str()));
      list[i].set_age(std::atoi(hash["age"].c_str()));
      list[i].set_signature(hash["id"]);
    }
    return true;
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
    return false;
  }
}

bool RedisStore::insertManage(bool initiative, bool op, const Basic& basic){
  try {
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
  }
}

bool RedisStore::getManage(int id, std::vector<Basic>& list){
  try {
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
  }
}

bool RedisStore::getManage(int id, std::vector<int>& list){
  try {
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
  }
}

bool RedisStore::deleteManage(int id){
  try {
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
  }
}

bool RedisStore::insertAdd(int id, int friendid){
  try {
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
  }
}

bool RedisStore::insertMsg(const Msg& msg){
  try {
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
  }
}

bool RedisStore::getMsg(int id, std::vector<Msg>& list){
  try {
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
  }
}

bool RedisStore::deleteMsg(int id){
  try {
  } catch (const Error &e) {
    LOG(INFO) << "check redis error" << e.what();
  }
}

} // namespace vchat
