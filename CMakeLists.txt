cmake_minimum_required(VERSION 2.8)

project(Msg)

add_subdirectory(sock)

add_library(
    libMsg
    STATIC
    Source/MsgClient.cpp
    Source/MsgTcpIpConnection.cpp
    Source/MsgTcpIpServer.cpp
    Source/MsgService.cpp
    Source/MsgHandler.cpp
)

target_include_directories(
    libMsg
    PUBLIC
    Include
    sock/Include
    C:/Users/pikwiatk/boost_1_85_0
#    C:/Users/pikwiatk/googletest-release-1.6.0/include
#    C:/Users/pikwiatk/googlemock-release-1.6.0/include
)

add_executable(
    MsgServerEx
    Source/ServerAppEx.cpp
)

target_include_directories(
    MsgServerEx
    PUBLIC
    Include
    sock/Include
    C:/Users/pikwiatk/boost_1_85_0
#    C:/Users/pikwiatk/googletest-release-1.6.0/include
#    C:/Users/pikwiatk/googlemock-release-1.6.0/include
)

add_dependencies(
    MsgServerEx
    libMsg
    libSock
)

target_link_libraries(
    MsgServerEx
    libMsg
    libSock
    ws2_32
)

add_executable(
    MsgClientEx
    Source/ClientAppEx.cpp
)

target_include_directories(
    MsgClientEx
    PUBLIC
    Include
    sock/Include
    C:/Users/pikwiatk/boost_1_85_0
#    C:/Users/pikwiatk/googletest-release-1.6.0/include
#    C:/Users/pikwiatk/googlemock-release-1.6.0/include
)

add_dependencies(
    MsgClientEx
    libMsg
    libSock
)

target_link_libraries(
    MsgClientEx
    libMsg
    libSock
    ws2_32
)

if( MSVC ) # VS2012 doesn't support correctly the tuples yet
    add_definitions( /D _VARIADIC_MAX=10 )
endif()
