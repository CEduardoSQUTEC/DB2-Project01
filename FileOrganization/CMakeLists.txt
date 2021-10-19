cmake_minimum_required(VERSION 3.20)
project(FileOrganization)

set(CMAKE_CXX_STANDARD 17)

add_executable(FileOrganization main.cpp ExpandibleHashing.cpp ExpandibleHashing.h Movie.cpp Movie.h const.h Bucket.h)
