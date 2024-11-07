#ifndef SERVICE_H
#define SERVICE_H

#include <functional>
#include <map>
#include "connection.h"
#include "singleton_crtp.h"
#include "vchat.pb.h"

using namespace vprotocol;

namespace vchat {

class Connection;
class Service : public SingletonCRTP<Service> {
public:
  void sign(RequestSign sign, const Connection_ptr& cp);
  void log(RequestLog log);
  void synchronous(RequestSynchronous sync);
  void change(RequestChange change);
  void find(RequestFind find, const Connection_ptr& cp);
  void manage(RequestManage manage);
  void msg(RequestMsg msg);

private:
  std::map<int, Connection_ptr> routingtable;

  // sign
  void signIn(RequestSign sign, const Connection_ptr& cp);
  void signOut(RequestSign sign, const Connection_ptr& cp);

  // log
  void logIn(RequestLog log);
  void getOffline(OfflineInfo& offlineinfo, int id);
  void logOut(RequestLog log);

  // change
  void changeBasic(RequestChange change);
  void changePersonal(RequestChange change);
  void changeFriend(RequestChange change);

  // managee
  void addFriend(RequestManage manage);
  void deleteFriend(RequestManage manage);

  // tools
  int generateId();
  void boardcast(int id, const std::function<void(int)>& callback);

};

} // namespace vchat

#endif // SERVICE_H
