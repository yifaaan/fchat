#pragma once

#include "const.h"
#include "singleton.h"

class IoContextPool : public Singleton<IoContextPool> {
 public:
  using IoContext = net::io_context;
  using Work = net::executor_work_guard<net::io_context::executor_type>;
  using WorkPtr = std::unique_ptr<Work>;

  IoContextPool(const IoContextPool&) = delete;
  IoContextPool& operator=(const IoContextPool&) = delete;
  IoContextPool(IoContextPool&&) = delete;
  IoContextPool& operator=(IoContextPool&&) = delete;
  ~IoContextPool();

  void Stop();

  net::io_context& GetIoContext();

 private:
  friend class Singleton<IoContextPool>;
  IoContextPool(size_t size = 4);

  std::vector<IoContext> io_contexts_;
  std::vector<WorkPtr> works_;
  std::vector<std::thread> threads_;
  size_t next_io_context_{0};
};