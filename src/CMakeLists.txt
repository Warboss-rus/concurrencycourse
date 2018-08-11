

add_executable(1_thread 1_thread.cpp)
custom_enable_cxx17(1_thread)

add_executable(2_async x_start.cpp 2_async.cpp)
custom_enable_cxx17(2_async)
target_link_libraries(2_async PRIVATE benchmark)

add_executable(3_mutex 3_mutex.cpp JoinableThread.h)
custom_enable_cxx17(3_mutex)

add_executable(4_concurrent_map 4_concurrent_map.cpp concurrent_map.h)
custom_enable_cxx17(4_concurrent_map)

add_executable(5_concurrent_map_2 x_start.cpp 5_concurrent_map_2.cpp concurrent_map.h)
custom_enable_cxx17(5_concurrent_map_2)
target_link_libraries(5_concurrent_map_2 PRIVATE benchmark)

add_executable(6_deadlock_detection 6_deadlock_detection.cpp hierarchical_mutex.h hierarchical_mutex.cpp Logger.h)
custom_enable_cxx17(6_deadlock_detection)

add_executable(7_condition_variable 7_condition_variable.cpp JoinableThread.h)
custom_enable_cxx17(7_condition_variable)

add_executable(8_thread_safe_queue x_start.cpp 8_thread_safe_queue.cpp OptimizedThreadSafeQueue.h SimpleThreadSafeQueue.h JoinableThread.h)
custom_enable_cxx17(8_thread_safe_queue)
target_link_libraries(8_thread_safe_queue PRIVATE benchmark)

add_executable(9_thread_pool x_start.cpp 9_thread_pool.cpp ThreadPool.h JoinableThread.h)
custom_enable_cxx17(9_thread_pool)
target_link_libraries(9_thread_pool PRIVATE benchmark)
