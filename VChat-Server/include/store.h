#ifndef STORE_H
#define STORE_H

#include <sw/redis++/redis++.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include "singleton_crtp.h"
#include "vchat.pb.h"

using namespace sw::redis;
using namespace vprotocol;

namespace vchat {

class RedisStore : public SingletonCRTP<RedisStore> {
private:
  std::unique_ptr<Redis> redis;
  RedisStore(std::string redis_port);

public:
  bool add(int id);
  bool remove(int id);

  // signout offline info
  bool insertSign(int id_, int id);

  // online status
  bool checkOnline(int id);
  bool setOnline(int id, bool online);

  // change offline info
  bool insertChange(int id, const Basic& basic);
  bool getChange(int id, std::vector<Basic>& list);
  bool deleteChange(int id);

  // manage(add/delete) offline info
  // for manage notify
  bool insertManage(bool initiative, bool op, const Basic& basic);
  bool getManage(int id, std::vector<Basic>& list);
  bool getManage(int id, std::vector<int>& list);
  bool deleteManage(int id);
  bool insertAdd(int id, int friendid);

  // message offline info
  bool insertMsg(const Msg& msg);
  bool getMsg(int id, std::vector<Msg>& list);
  bool deleteMsg(int id);

};

class SqliteStore : public SingletonCRTP<SqliteStore> {
private:
  std::unique_ptr<SQLite::Database> db;
  SqliteStore(std::string db_address);

public:
  static void InitSqlite();
  bool checkId(int id); // used by log in
  bool checkPassword(int id, int password); // used by log in

  // basic table
  bool getBasic(int id, Basic& basic); // used by find, synchronous
  bool getBasic(std::string name, std::vector<Basic>& basic); // used by find
  bool insertBasic(const Basic& basic); // used by sign in
  bool changeBasic(const Basic& basic); // used by change basic
  bool deleteBasic(int id); // used by sign out

  // personal table
  bool getPersonal(int id, Personal& info); // used by synchronous
  bool insertPersonal(const Personal& personal); // used by sign in
  bool changePersonal(const Personal& personal); // used by change personal
  bool deletePersonal(int id); // used by sign out

  // friend table
  bool getFriends(int id, std::vector<Friend>& friends); // used by boardcast, synchronous
  bool insertFriend(int id, const Friend& friend_); // uesd by add friend
  bool changeFriend(const Friend& friend_); // used by change friend
  bool deleteFriend(int id); // used by sign out
  bool deleteFriend(int id, int friendid); // used by delete friend

  // msg table
  bool getMsg(int id, std::vector<Msg>& msgs); // id is user's id
  bool insertMsg(Msg& msg); // used by msg
  bool deleteMsg(int id); // used by sign out
  bool deleteMsg(int sender, int receiver); // used by delete friend

};

} // namespace vchat

#endif // STORE_H
