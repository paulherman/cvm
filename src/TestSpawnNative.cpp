#include <thread>
#include <iostream>
#include <ctime>
#include <vector>
#include "Build.h"

void worker() {
}

void test(size_t numWorkers) {
  std::clock_t start = std::clock();

  std::vector<std::thread> workers;
  for (size_t i = 0; i < numWorkers; i++) {
    std::thread t(worker);
    workers.push_back(std::move(t));
  }

  for (std::thread &t : workers)
    t.join();

  std::clock_t end = std::clock();
  std::cout << double(end - start) / CLOCKS_PER_SEC  << '\n'; 
}

int main(int argc, char **argv) {
  for (size_t numWorkers = 1; numWorkers < TEST_MAX_NUM_SPAWNS; numWorkers *= 2)
    test(numWorkers);
  return 0;
}
