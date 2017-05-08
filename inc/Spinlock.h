#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <atomic>
#include <thread>

class Spinlock {
public:
  Spinlock();
  void lock();
  void unlock();
private:
  std::thread::id nonThreadId;
  std::atomic<std::thread::id> currentThread;
};

#endif
