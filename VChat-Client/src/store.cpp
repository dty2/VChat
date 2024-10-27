#include <cstdlib>
#include <string>
#include <filesystem>
#include "store.h"
#include "data.hpp"

Store *Store::store = nullptr;
void Store::getinstance(int id) { store = new Store(id); }
void Store::free() { delete store; }

Store::Store(int id) {
  std::string home = std::getenv("HOME");
  std::filesystem::path cache_path(home + ".config/vchat/");
  if(!std::filesystem::exists(cache_path)) std::filesystem::create_directories(cache_path);
  std::string cache_file = cache_path.string() + std::to_string(id) + ".db";
  if(!std::filesystem::exists(cache_file)) {
    std::filesystem::create_directories(cache_file);
    SQLite::Database db(SQLite::Database(db_address, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE));
      try {
        db.exec(R"(
          CREATE TABLE persional (
              id INT PRIMARY KEY,
              passwd INT NOT NULL,
              nickname TEXT DEFAULT "昵称",
              gender INT DEFAULT 2, -- man 0, woman 1, unknow 2
              age INT CHECK(age >= 0),
              signaure TEXT DEFAULT "沉默是金"
          )
        )");
        db.exec(R"(
          CREATE TABLE friends(
              id INT NOT NULL,
              nickname TEXT DEFAULT "昵称",
              gender INT DEFAULT 2,
              age INT CHECK(age >= 0),
              signaure TEXT DEFAULT "沉默是金"
          );
        )");
        db.exec(R"(
          CREATE TABLE messages(
              sender INT NOT NULL,
              receiver INT NOT NULL,
              message TEXT NOT NULL,
              timer BIGINT NOT NULL
          );
        )");
      } catch (std::exception &e) {
      }
  } else {
    SQLite::Database db(SQLite::Database(db_address, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE));
  }
  this->db = std::move(db);
}

bool Store::getPersional() {
  try {
    SQLite::Statement query(this->db, "SELECT * FROM persionalinfo WHERE id = ?");
    query.bind(1, persionalinfo.info.id);
    while (query.executeStep()) {
      persionalinfo.info.id = query.getColumn(0).getInt();
      persionalinfo.password = query.getColumn(1).getInt();
      persionalinfo.info.nickname = query.getColumn(2).getString();
      persionalinfo.info.gender = query.getColumn(3).getInt();
      persionalinfo.info.age = query.getColumn(4).getInt();
      persionalinfo.info.signature = query.getColumn(5).getString();
    }
    return true;
  } catch (std::exception &e) {
    return false;
  }
}

bool Store::getFriend() {
  try {
    SQLite::Statement query(this->db, "SELECT * FROM friends");
    query.bind(1, persionalinfo.info.id);
    while (query.executeStep()) {
      int id = query.getColumn(0).getInt();
      friends[id].info.id = query.getColumn(0).getInt();
      friends[id].info.nickname = query.getColumn(1).getString();
      friends[id].info.gender = query.getColumn(2).getInt();
      friends[id].info.age = query.getColumn(3).getInt();
      friends[id].info.signature = query.getColumn(4).getString();
    }
    return true;
  } catch (std::exception &e) {
    return false;
  }
}

bool Store::getMessage() {
  try {
    SQLite::Statement query(this->db, "SELECT * FROM messages");
    while (query.executeStep()) {
      MessageInfo msg;
      msg.sender = query.getColumn(0).getInt();
      msg.receiver = query.getColumn(1).getInt();
      msg.msg = query.getColumn(2).getString();
      msg.time = query.getColumn(3).getInt64();
      if (msg.sender == persionalinfo.info.id)
        msgs[msg.receiver].emplace_back(msg);
      else msgs[msg.sender].emplace_back(msg);
    }
    return true;
  } catch (std::exception &e) {
    return false;
  }
}

bool Store::loadInfo() {
  bool sign1 = getPersional();
  bool sign2 = getFriend();
  bool sign3 = getMessage();
  return sign1 && sign2 && sign3;
}

bool Store::changeInfo(const PersionalInfo &info) {
  try {
    SQLite::Statement query(this->db, "INSERT OR REPLACE INTO persionalinfo VALUES (?, ?, ?, ?, ?, ?)");
    query.bind(1, info.info.id);
    query.bind(2, info.password);
    query.bind(3, info.info.nickname);
    query.bind(4, info.info.gender);
    query.bind(5, info.info.age);
    query.bind(6, info.info.signature);
    query.exec();
    return true;
  } catch (std::exception &e) {
    return false;
  }
}

bool Store::changeInfo(const FriendInfo &info) {
  try {
    SQLite::Statement query(this->db, "INSERT OR REPLACE INTO friends VALUES (?, ?, ?, ?, ?)");
    query.bind(1, info.info.id);
    query.bind(2, info.info.nickname);
    query.bind(3, info.info.gender);
    query.bind(4, info.info.age);
    query.bind(5, info.info.signature);
    query.exec();
    return true;
  } catch (std::exception &e) {
    return false;
  }
}

bool Store::deleteFriend(const int id) {
  try {
    SQLite::Statement query(this->db, "DELETE FROM friendinfo WHERE id = ?");
    query.bind(1, id);
    query.exec();
    SQLite::Statement query2(this->db, "DELETE FROM messageinfo WHERE sender = ? OR receiver = ?");
    query.bind(1, id);
    query.exec();
    return true;
  } catch (std::exception &e) {
    return false;
  }
}

bool Store::insertMessage(const MessageInfo &messageinfo) {
  try {
    SQLite::Statement query(this->db, "INSERT OR REPLACE INTO messageinfo VALUES (?, ?, ?, ?)");
    query.bind(1, messageinfo.sender);
    query.bind(2, messageinfo.receiver);
    query.bind(3, messageinfo.msg);
    query.bind(4, messageinfo.time);
    query.exec();
    return true;
  } catch (std::exception &e) {
    return false;
  }
}

bool Store::storeInfo() {
  try {
    changeInfo(persionalinfo);
    // value == true for change or add, false for delete
    for(auto &[index, value] : friends) {
      if(friends[index].info.status == -1) deleteFriend(index);
      else changeInfo(friends[index]);
    }
    for(auto &[index, value] : msgs) {
      for(auto &v : msgs[index])
        insertMessage(v);
    }
    return true;
  } catch (std::exception &e) {
    return false;
  }
}
