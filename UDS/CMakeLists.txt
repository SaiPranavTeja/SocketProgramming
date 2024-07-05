cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

# Set the version number
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

# Add the include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add the source files
add_executable(Client 
    Client.cc
)

add_executable(Server 
    Server.cc
)
