#ifndef STORE_H
#define STORE_H

#include "data.hpp"
#include <string>
#include <vector>

class Store {
private:
  std::string sql;
  std::unique_ptr<SQLite::Database> db;

public:
  Store(std::string sql);
  ~Store();

  bool getPersional(PersionalInfo&, const int);
  bool getPersional(std::vector<PersionalInfo>&, const std::string);
  bool getFriend(std::list<FriendInfo>&, const int);
  bool getMessage(std::list<MessageInfo>&, const int);
  bool getUser(UserInfo&, const int);
  bool insertPersional(PersionalInfo&);
  bool insertFriend(FriendInfo&, int);
  bool insertMessage(MessageInfo&);
};

#endif // STORE_H
