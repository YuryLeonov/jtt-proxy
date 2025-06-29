cmake_minimum_required(VERSION 3.5)

project(mtp-808-proxy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/binDebug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG /opt/mtp/mtp-808-proxy/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/binRelease)

find_package(websocketpp REQUIRED)

#add_definitions(-DTEST_MODE)

include_directories(
        ./
        ./src
        ./nlohmann
        ./easylogging
        ./websocket
        ./jt808
        ./tests
    )

set(HEADERS ./nlohmann/json.hpp
            ./easylogging/easylogging++.h
            ./src/configurationparser.h
            ./src/confmodificationwatcher.h
            ./src/configuration.h
            ./src/module.h
            ./src/tools.h
            ./src/requestsmap.h
            ./src/eventrequests.h
            ./src/terminalinfo.h
            ./src/terminalparams.h
            ./src/videoserverinfo.h
            ./src/mediatransmissionparams.h
            ./src/platforminfo.h
            ./src/eventserverinfo.h
            ./jt808/jt808serializer.h
            ./jt808/jt808client.h
            ./jt808/jt808server.h
            ./jt808/jt808header.h
            ./jt808/jt808headerparser.h
            ./jt808/jt808messageformatter.h
            ./jt808/jt808registrationrequest.h
            ./jt808/jt808authenticationrequest.h
            ./jt808/jt808heartbeatrequest.h
            ./jt808/terminallogoutrequest.h
            ./jt808/jt808generalresponserequest.h
            ./jt808/jt808terminalparametersrequest.h
            ./jt808/jt808mediauploadrequest.h
            ./jt808/jt808authenticationkeyfinder.h
            ./jt808/jt808mediauploadeventinforequest.h
            ./jt808/jt1078mediatransmissionrequest.h
            ./websocket/websocketclient.h
            ./websocket/websocketserver.h

            ./tests/jt808serializer.h
            ./tests/jt808requeststest.h
            )

set(SOURCES ./src/main.cpp
            ./src/tools.cpp
            ./src/eventrequests.cpp
            ./src/module.cpp
            ./src/configurationparser.cpp
            ./src/confmodificationwatcher.cpp
            ./jt808/jt808serializer.cpp
            ./jt808/jt808server.cpp
            ./jt808/jt808client.cpp
            ./jt808/jt808headerparser.cpp
            ./jt808/jt808serializer.cpp
            ./jt808/jt808messageformatter.cpp
            ./jt808/jt808registrationrequest.cpp
            ./jt808/jt808generalresponserequest.cpp
            ./jt808/jt808authenticationrequest.cpp
            ./jt808/jt808heartbeatrequest.cpp
            ./jt808/jt808terminallogoutrequest.cpp
            ./jt808/jt808terminalparametersrequest.cpp
            ./jt808/jt808mediauploadrequest.cpp
            ./jt808/jt808authenticationkeyfinder.cpp
            ./jt808/jt808mediauploadeventinforequest.cpp
            ./jt808/jt1078mediatransmissionrequest.cpp
            ./websocket/websocketclient.cpp
            ./websocket/websocketserver.cpp
            ./easylogging/easylogging++.cc)


add_executable(mtp-808-proxy ${SOURCES})
target_link_libraries(mtp-808-proxy pthread systemd)
