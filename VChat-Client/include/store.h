#ifndef STORE_H
#define STORE_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <unordered_map>
#include <vector>
#include "data.hpp"

class Store {
public:
  // static data
  PersionalInfo persionalinfo;
  std::unordered_map<std::string, int> nickname_id;
  std::unordered_map<int, FriendInfo> friends;
  std::unordered_map<int, std::vector<MessageInfo>> msgs;
  std::list<int> add;

  // dynamic data
  std::unordered_map<int, FriendInfo> finds;

  static Store *store;
  static void getinstance(int id);
  static void free();
  Store(const Store&) = delete;
  Store& operator=(const Store&) = delete;

  bool loadInfo();
  bool synchronous();
  bool roaming();
  bool storeInfo();

private:
  SQLite::Database db = nullptr;

  std::string db_address;
  explicit Store(int id);

  bool getPersional();
  bool getFriend();
  bool getMessage();

  bool changeInfo(const PersionalInfo &info);
  bool changeInfo(const FriendInfo &info);
  bool deleteFriend(const int id);
  bool insertMessage(const MessageInfo &msg);

};

#endif // STORE_H
