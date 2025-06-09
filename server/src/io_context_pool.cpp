#include "io_context_pool.h"

IoContextPool::IoContextPool(size_t size) : io_contexts_{size}, works_{size} {
  threads_.reserve(size);
  for (size_t i = 0; i < size; i++) {
    works_[i] = std::make_unique<Work>(io_contexts_[i].get_executor());
    threads_.emplace_back([this, i] { io_contexts_[i].run(); });
  }
}

IoContextPool::~IoContextPool() {
  Stop();
  std::cout << "IoContextPool::~IoContextPool()" << std::endl;
}

net::io_context& IoContextPool::GetIoContext() { return io_contexts_[next_io_context_++ % io_contexts_.size()]; }

void IoContextPool::Stop() {
  for (auto& ioc : io_contexts_) {
    ioc.stop();
  }
  for (auto& work : works_) {
    work.reset();
  }
  for (auto& thread : threads_) {
    thread.join();
  }
}