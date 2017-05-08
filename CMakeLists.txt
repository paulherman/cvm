cmake_minimum_required (VERSION 2.6)

project(cvm)

add_compile_options(-std=c++11 -Wall)

include_directories(inc)

link_directories(/usr/local/lib64)

add_library(cvm src/Program.cpp src/Process.cpp src/IdGen.cpp src/Type.cpp src/Value.cpp src/Const.cpp src/Function.cpp src/Instr.cpp src/BinaryReader.cpp)
target_link_libraries(cvm pthread dl boost_context libcds.so)

add_executable(test_native_spawn src/TestSpawnNative.cpp)
target_link_libraries(test_native_spawn pthread)

add_executable(test_vm_spawn src/TestSpawnVm.cpp)
target_link_libraries(test_vm_spawn cvm)

add_executable(test_vm_message src/TestMessageVm.cpp)
target_link_libraries(test_vm_message cvm)

add_executable(test_vm_ping_pong src/TestPingPongVm.cpp)
target_link_libraries(test_vm_ping_pong cvm)

add_executable(test_vm_gc src/TestGc.cpp)
target_link_libraries(test_vm_gc cvm)

add_executable(test_vm_message_large src/TestMessageLarge.cpp)
target_link_libraries(test_vm_message_large cvm)

add_executable(test_vm_yield src/TestSwitching.cpp)
target_link_libraries(test_vm_yield cvm)
