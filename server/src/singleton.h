#pragma once

#include <iostream>
#include <memory>
#include <mutex>

template <typename T>
class Singleton {
 protected:
  Singleton() = default;
  Singleton(const Singleton&) = delete;
  Singleton(Singleton&&) = delete;
  Singleton& operator=(Singleton&&) = delete;

  static inline std::shared_ptr<T> instance_{};

 public:
  static std::shared_ptr<T> GetInstance() {
    static std::once_flag flag;
    std::call_once(flag, [&] { instance_ = std::shared_ptr<T>(new T); });
    return instance_;
  }

  void PrintAddress() { std::cout << instance_.get() << std::endl; }

  ~Singleton() {}
};
