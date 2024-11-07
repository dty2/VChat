#ifndef SINGLETON_CRTP_H
#define SINGLETON_CRTP_H

#include <memory>
#include <string>

namespace vchat {

template <typename T>
class SingletonCRTP {
public:
  static std::shared_ptr<T> getInstance(std::string arg1 = "") {
    static std::shared_ptr<T> instance = std::make_shared<T>(arg1);
    return instance;
  }

protected:
  SingletonCRTP() {}
  ~SingletonCRTP() {}

private:
  SingletonCRTP(const SingletonCRTP &) = delete;
  SingletonCRTP &operator=(const SingletonCRTP &) = delete;
};

} // namespace vchat

#endif // SINGLETON_CRTP_H
