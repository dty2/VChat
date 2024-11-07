#include <memory>
#include <string>
#include <filesystem>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include <glog/logging.h>

#include "server.h"
#include "service.h"
#include "store.h"

using namespace vchat;

ABSL_FLAG(std::string, port, "2500", "Default: 2500");
ABSL_FLAG(std::string, redis, "6379", "Default: 6379");
ABSL_FLAG(std::string, sqlite, "/var/tmp/vchat/vchat.db", "Default: /var/lib/vchat/vchat.db");
ABSL_FLAG(std::string, log, "/var/tmp/vchat/", "Default: /var/tmp/vchat/");

std::shared_ptr<RedisStore> redis;
std::shared_ptr<SqliteStore> sqlite;
std::shared_ptr<Service> service;
std::shared_ptr<Server> server;

class Init {
public:
  std::string net_port;
  std::string redis_port, db_address;
  std::string log_address;
  Init(int argc, char **argv) {
    absl::ParseCommandLine(argc, argv);
    // port init
    net_port = absl::GetFlag(FLAGS_port);
    redis_port = absl::GetFlag(FLAGS_redis);

    // log init
    log_address = absl::GetFlag(FLAGS_log);
    FLAGS_alsologtostderr = false;
    FLAGS_log_dir = log_address;
    FLAGS_max_log_size = 100 * 1024; // 100MB
    FLAGS_minloglevel = google::INFO;
    if(std::filesystem::exists(log_address))
      std::filesystem::create_directory(log_address);

    // sql init
    db_address = absl::GetFlag(FLAGS_sqlite);
    if(std::filesystem::exists(db_address)) {
      std::filesystem::create_directory(db_address);
      sqlite = SqliteStore::getInstance(db_address);
      SqliteStore::InitSqlite();
    } else {
      sqlite = SqliteStore::getInstance(db_address);
    }
    redis = RedisStore::getInstance(redis_port);
  }

  void startlog() {
    google::InitGoogleLogging("VChat-Server");
  }

  void stoplog() {
    google::ShutdownGoogleLogging();
  }

};

int main(int argc, char **argv) {
  Init init(argc, argv);
  init.startlog();
  LOG(INFO) << "VChat-Server start...";
  try {
    service = Service::getInstance();
    server = Server::getInstance(init.net_port);
  } catch (const std::exception &e) {
    LOG(INFO) << e.what();
  }
  init.stoplog();
  return 0;
}
