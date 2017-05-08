#include "Spinlock.h"

Spinlock::Spinlock()
  : nonThreadId(), currentThread(nonThreadId) {
}

void Spinlock::lock() {
  std::thread::id myId = std::this_thread::get_id();

  bool done = false;
  do {
    std::thread::id currentId = currentThread.load();
    if (currentId == myId || currentId == nonThreadId) {
      done = currentThread.compare_exchange_weak(currentId, myId);
    }
  } while (!done);
}

void Spinlock::unlock() {
  std::thread::id currentId = currentThread.load();
  std::thread::id myId = std::this_thread::get_id();
  if (currentId == myId)
    currentThread.compare_exchange_strong(myId, myId);
}
