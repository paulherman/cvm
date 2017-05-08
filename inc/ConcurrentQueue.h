#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include <boost/optional.hpp>
#include <atomic>
#include <memory>

template<typename T>
class CNNode {
public:
  CNNode(const T &value, CNNode<T> *next);
  CNNode(CNNode<T> *next);
  const T value;
  std::atomic<CNNode<T> *> next;
};

template<typename T>
class ConcurrentQueue {
public:
  ConcurrentQueue();
  void enqueue(const T &value);
  boost::optional<T> dequeue();
  T dequeueBlock();
  bool empty();

private:
  std::atomic<CNNode<T> *> head;
  std::atomic<CNNode<T> *> tail;
};

template<typename T>
CNNode<T>::CNNode(const T &value, CNNode<T> *nextPtr)
  : value(value) {
  next.store(nextPtr);
}

template<typename T>
CNNode<T>::CNNode(CNNode<T> *nextPtr) {
  next.store(nextPtr);
}

template<typename T>
ConcurrentQueue<T>::ConcurrentQueue() {
  CNNode<T> *dummy = new CNNode<T>(nullptr);
  head.store(dummy);
  tail.store(dummy);
}

template<typename T>
void ConcurrentQueue<T>::enqueue(const T &value) {
  CNNode<T> *node = new CNNode<T>(value, nullptr);
  while (true) {
    CNNode<T> *myTail = tail.load();
    CNNode<T> *next = myTail->next.load();
    if (myTail == tail.load()) {
      if (next == nullptr) {
        if (myTail->next.compare_exchange_weak(next, node)) {
          tail.compare_exchange_strong(myTail, node);
          return;
        }
      } else {
        tail.compare_exchange_weak(myTail, next);
      }
    }
  }
}

template<typename T>
boost::optional<T> ConcurrentQueue<T>::dequeue() {
  while (true) {
    CNNode<T> *myHead = head.load();
    CNNode<T> *myTail = tail.load();
    CNNode<T> *next = myHead->next.load();

    if (myHead == head.load()) {
      if (myHead == myTail) {
        if (next == nullptr) {
          return boost::none;
        } else {
          tail.compare_exchange_weak(myTail, next);
        }
      } else {
        if (head.compare_exchange_weak(myHead, next)) {
          boost::optional<T> result(next->value);
          delete next; // TODO: check for sigsegv
          return result;
        }
      }
    }
  }
}

template<typename T>
bool ConcurrentQueue<T>::empty() {
  return head.load() == tail.load();
}

template<typename T>
T ConcurrentQueue<T>::dequeueBlock() {
  while (true) {
    CNNode<T> *myHead = head.load();
    CNNode<T> *myTail = tail.load();
    CNNode<T> *next = myHead->next.load();

    if (myHead == head.load()) {
      if (myHead == myTail) {
        if (next != nullptr)
          tail.compare_exchange_weak(myTail, next);
      } else {
        if (head.compare_exchange_weak(myHead, next)) {
          boost::optional<T> result(next->value);
          delete next; // TODO: check for sigsegv
          return result;
        }
      }
    }
  }
}

#endif
