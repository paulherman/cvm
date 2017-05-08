#ifndef BUILD_H
#define BUILD_H

#define BUILD_DIR "/home/paulsh94/Workspace/cvmr/"
#define HEAP_SIZE 1024
#define MAX_NUM_MESSAGES 128
#define MAX_NUM_PROCESSES 20480
#define DEBUG(fmt, ...) { fprintf(stderr, fmt, __VA_ARGS__); }
#define MEASURE(f) { std::clock_t start = std::clock(); { f; } std::clock_t end = std::clock(); std::cout << "Time taken " << double(end - start) / CLOCKS_PER_SEC << '\n'; }
#define TEST_MAX_NUM_SPAWNS 20000

#endif
