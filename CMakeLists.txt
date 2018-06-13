cmake_minimum_required(VERSION 2.8)

project(Msg)

link_directories(
    D:/boost_1_61_0/stage/lib
    D:/googletest-release-1.6.0/Build/Debug
    D:/googlemock-release-1.6.0/Build/Debug
    D:/test/sock/BuildMS/Release
)

add_library(
    libMsg
    STATIC
    Source/MsgClient.cpp
    Source/MsgTcpIpConnection.cpp
    Source/MsgTcpIpServer.cpp
    Source/MsgService.cpp
)

target_include_directories(
    libMsg
    PUBLIC
    Include
    ../sock/Include
    D:/boost_1_61_0
    D:/googletest-release-1.6.0/include
    D:/googlemock-release-1.6.0/include
)

add_executable(
    ServerEx
    Source/ServerAppEx.cpp
)

target_include_directories(
    ServerEx
    PUBLIC
    Include
    ../sock/Include
    D:/boost_1_61_0
    D:/googletest-release-1.6.0/include
    D:/googlemock-release-1.6.0/include
)

add_dependencies(
    ServerEx
    libMsg
)

target_link_libraries(
    ServerEx
    libMsg
    libSock.lib
    ws2_32
)

add_executable(
    ClientEx
    Source/ClientAppEx.cpp
)

target_include_directories(
    ClientEx
    PUBLIC
    Include
    ../sock/Include
    D:/boost_1_61_0
    D:/googletest-release-1.6.0/include
    D:/googlemock-release-1.6.0/include
)

add_dependencies(
    ClientEx
    libMsg
)

target_link_libraries(
    ClientEx
    libMsg
    libSock.lib
    ws2_32
)

if( MSVC ) # VS2012 doesn't support correctly the tuples yet
    add_definitions( /D _VARIADIC_MAX=10 )
endif()
