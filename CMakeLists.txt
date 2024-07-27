cmake_minimum_required(VERSION 2.20) 

project(ProjectPostgreCXX) 

set(PostgreSQL_AUDDITIONAL_VERSIONS "16")

add_subdirectory("C:\\work\\libpqxx-7.9.1" libpqxx-build)

add_executable(ProjectPostgreCXX main.cpp) 

target_compile_features(ProjectPostgreCXX PRIVATE cxx_std_17) 

target_link_libraries(ProjectPostgreCXX PRIVATE pqxx) 