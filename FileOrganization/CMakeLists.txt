cmake_minimum_required(VERSION 3.17)
project(FileOrganization)

set(CMAKE_CXX_STANDARD 17)

add_executable(FileOrganization main.cpp libs.h
        ExtendibleHashing/ExpandibleHashing.h
        ExtendibleHashing/Bucket.h
        SequentialFile/SeqFile.h
        SequentialFile/SeqFile.cpp
        Record/Movie.cpp
        Record/Movie.h
        Record/Netflix.h
        Record/Basket.h
        Record/WorldPopulation.h
        Test/menu.h
        )
