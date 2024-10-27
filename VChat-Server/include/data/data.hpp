// #ifndef USERINFO_HPP
// #define USERINFO_HPP
//
// #include "common.h"
//
// typedef struct MessageInfo {
//   int sender;
//   int receiver;
//   std::string msg;
//   int64_t time;
//   MessageInfo(){};
//   MessageInfo(int sender_, int receiver_, std::string msg_, std::time_t time_) :
//     sender(sender_),
//     receiver(receiver_),
//     msg(msg_),
//     time(time_) {}
// }MessageInfo;
//
// typedef struct FriendInfo {
//   int friendid;
//   std::string friendname;
//   FriendInfo(){};
//   FriendInfo(int id_, std::string friendname_) : friendid(id_), friendname(friendname_) {}
// }FriendInfo;
//
// typedef struct PersionalInfo {
//   int id;
//   int password;
//   std::string username;
//   PersionalInfo(){}
//   PersionalInfo(int id_, int password_, std::string username_) :
//     id(id_),
//     password(password_),
//     username(username_) {}
// }PersionalInfo;
//
// typedef struct UserInfo {
//   PersionalInfo persionalinfo;
//   std::list<FriendInfo> friendlist;
//   std::list<MessageInfo> messagelist;
//   UserInfo(){};
// }UserInfo;
//
// #endif // USERINFO_HPP
//
