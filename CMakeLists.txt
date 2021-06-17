cmake_minimum_required(VERSION 3.10)

project(rhpc-classic-pp)

find_package(Boost 1.58 REQUIRED COMPONENTS mpi system)
find_package(MPI REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG, "${CMAKE_CXX_FLAGS_DEBUG} -O0")

add_executable(rhpc-classic-pp
	main.cpp
	src/model.cpp
	src/agent.cpp
	src/grass.cpp
	src/package.cpp
	src/output.cpp)

target_include_directories(rhpc-classic-pp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(rhpc-classic-pp
	MPI::MPI_CXX Boost::boost Boost::system Boost::mpi repast_hpc-2.3.0
	)
