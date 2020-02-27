cmake_minimum_required(VERSION 3.14)

project(hw)

# set(Boost_USE_STATIC_LIBS OFF)
# set(Boost_USE_MULTITHREADED ON)
# set(Boost_USE_STATIC_RUNTIME OFF)
# find_package(Boost COMPONENTS coroutine asio range system)

find_package(gflags)

# include_directories(${Boost_INCLUDE_DIRS})
# message(${Boost_INCLUDE_DIRS})
add_executable(run main.cc ib.cc discovery.cc)
target_link_libraries(run pthread rdmacm ibverbs memcached)
