#ifndef DATA_H
#define DATA_H

#include "ctos.pb.h"
#include <string>
#include <ctime>

using MessageInfo = struct MessageInfo {
  int sender;
  int receiver;
  std::string msg;
  int64_t time;
};

using Info = struct Info {
  int id;
  std::string nickname;
  bool gender;
  int age;
  // -1 have been deleted, 0 not online, 1 online
  int status; // dynamic data, do not need to store in sql
  std::string signature;
  void operator=(const vchat::PersionalInfo &persionalinfo) {
    id = persionalinfo.id();
    nickname = persionalinfo.nickname();
    age = persionalinfo.age();
    gender = persionalinfo.gender();
    signature = persionalinfo.signature();
    status = persionalinfo.status();
  }
};

using FriendInfo = struct FriendInfo {
  Info info;
  void operator=(const vchat::PersionalInfo &persionalinfo) {
    info = persionalinfo;
  }
};

using PersionalInfo = struct PersionalInfo {
  Info info;
  int password;
  void operator=(const vchat::PersionalInfo &persionalinfo) {
    info = persionalinfo;
    password = persionalinfo.password();
  }
};

#endif // DATA_H
