#include <string>
#include <filesystem>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include <glog/logging.h>

#include "connection.h"
#include "server.h"

using namespace vchat;

ABSL_FLAG(std::string, port, "2500", "Default: 2500");
ABSL_FLAG(std::string, redis, "6379", "Default: 6379");
ABSL_FLAG(std::string, sql, "/var/vchat/sql", "Default: /var/vchat/sql");
ABSL_FLAG(std::string, log, "/var/vchat/log", "Default: /var/vchat/log");

class Init {
public:
  std::string port, redis, sql, log;
  Init(int argc, char **argv) {
    absl::ParseCommandLine(argc, argv);
    // port init
    std::string port = absl::GetFlag(FLAGS_port),
                redis = absl::GetFlag(FLAGS_redis);

    // log init
    std::string log = absl::GetFlag(FLAGS_log);
    FLAGS_alsologtostderr = false;
    FLAGS_log_dir = log;
    FLAGS_max_log_size = 100 * 1024; // 100MB
    FLAGS_minloglevel = google::INFO;
    if(std::filesystem::exists("/var/vchat/log"))
      std::filesystem::create_directory("/var/vchat/log");

    // sql init
    std::string sql = absl::GetFlag(FLAGS_sql);
    if(std::filesystem::exists("/var/vchat/sql"))
      std::filesystem::create_directory("/var/vchat/sql");
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
    // Store store(init.sql);
    ConnectionManager connectionmanager;
    Server server(connectionmanager, init.port);
    server.run();
  } catch (const std::exception &e) {
    LOG(INFO) << e.what();
  }
  init.stoplog();
  return 0;
}
