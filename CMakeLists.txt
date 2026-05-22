cmake_minimum_required(VERSION 4.2)
project(TPE_lucas)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(TPE_lucas
        .idea/.gitignore
        .idea/modules.xml
        .idea/TPE_lucas.iml
        .idea/vcs.xml
        .idea/workspace.xml
        B-tree.c
        B-tree.h
        main.c
        memoria.dat)
