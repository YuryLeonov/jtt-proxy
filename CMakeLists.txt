cmake_minimum_required(VERSION 3.5)

project(mtp-808-proxy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/binDebug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG /opt/lms/mtp-808-proxy/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/binRelease)

find_package(websocketpp REQUIRED)

include(cmake/FindLibAV.cmake)

#add_definitions(-DTEST_MODE)

set(AV_LIBRARIES avutil avcodec avfilter avformat)

include_directories(
        ./
        ./src
        ./nlohmann
        ./easylogging
        ./websocket
        ./jt808
        ./tests
        ./jsonrpc
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
            ./src/TerminalInfo.h
            ./src/terminalparams.h
            ./src/videoserverinfo.h
            ./src/mediatransmissionparams.h
            ./src/PlatformInfo.h
            ./src/eventserverinfo.h
            ./src/realtimevideostreamer.h
            /.src/alarmfileuploader.h
            ./src/alarmtypes.h
            ./src/platformalarmid.h
            ./src/logger.h
            ./jt808/jt808serializer.h
            ./jt808/jt808client.h
            ./jt808/jt808server.h
            ./jt808/jt808header.h
            ./jt808/jt808headerparser.h
            ./jt808/jt808messageformatter.h
            ./jt808/jt808messagevalidator.h
            ./jt808/jt808registrationrequest.h
            ./jt808/jt808authenticationrequest.h
            ./jt808/jt808alarmattachmentrequest.h
            ./jt808/jt808heartbeatrequest.h
            ./jt808/terminallogoutrequest.h
            ./jt808/jt808generalresponserequest.h
            ./jt808/jt808terminalparametersrequest.h
            ./jt808/jt808mediauploadrequest.h
            ./jt808/jt808authenticationkeyfinder.h
            ./jt808/jt808mediauploadeventinforequest.h
            ./jt808/jt808fileuploadinforequest.h
            ./jt808/jt808fileuploadstoprequest.h
            ./jt808/jt1078mediatransmissionrequest.h
            ./jt808/jt1078streamtransmitrequest.h
            ./jt808/jt1078uploadedresourceslistrequest.h
            ./websocket/websocketclient.h
            ./websocket/websocketserver.h
            ./jsonrpc/DbMessagesHelper.h
            ./jsonrpc/DbMessagesHelperOld.h
            ./jsonrpc/EntityError.h
            ./jsonrpc/files.h
            ./jsonrpc/uuid.h
            )

set(SOURCES ./src/main.cpp
            ./src/tools.cpp
            ./src/eventrequests.cpp
            ./src/module.cpp
            ./src/configurationparser.cpp
            ./src/confmodificationwatcher.cpp
            ./src/realtimevideostreamer.cpp
            ./src/alarmfileuploader.cpp
            ./jt808/jt808serializer.cpp
            ./jt808/jt808server.cpp
            ./jt808/jt808client.cpp
            ./jt808/jt808headerparser.cpp
            ./jt808/jt808serializer.cpp
            ./jt808/jt808messageformatter.cpp
            ./jt808/jt808messagevalidator.cpp
            ./jt808/jt808registrationrequest.cpp
            ./jt808/jt808generalresponserequest.cpp
            ./jt808/jt808authenticationrequest.cpp
            ./jt808/jt808heartbeatrequest.cpp
            ./jt808/jt808terminallogoutrequest.cpp
            ./jt808/jt808terminalparametersrequest.cpp
            ./jt808/jt808mediauploadrequest.cpp
            ./jt808/jt808authenticationkeyfinder.cpp
            ./jt808/jt808alarmattachmentrequest.cpp
            ./jt808/jt808mediauploadeventinforequest.cpp
            ./jt808/jt808fileuploadinforequest.cpp
            ./jt808/jt808fileuploadstoprequest.cpp
            ./jt808/jt1078mediatransmissionrequest.cpp
            ./jt808/jt1078streamtransmitrequest.cpp
            ./jt808/jt1078uploadedresourceslistrequest.cpp
            ./websocket/websocketclient.cpp
            ./websocket/websocketserver.cpp
            ./easylogging/easylogging++.cc
            ./jsonrpc/DbMessagesHelper.cpp
            ./jsonrpc/DbMessagesHelperOld.cpp
            ./jsonrpc/files.cpp
            ./jsonrpc/uuid.cpp

            )


add_executable(mtp-808-proxy ${SOURCES})
target_link_libraries(mtp-808-proxy pthread systemd ${AV_LIBRARIES})
