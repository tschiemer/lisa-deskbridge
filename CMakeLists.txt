cmake_minimum_required(VERSION 3.24)
project(lisa-deskbridge)

set(CMAKE_CXX_STANDARD 17)


include_directories(src/third_party/oscpack/)
add_subdirectory(src/third_party/oscpack EXCLUDE_FROM_ALL)


add_subdirectory(src/third_party/libremidi EXCLUDE_FROM_ALL)


include_directories(src/include/lisa-deskbridge)

add_library(lisa-deskbridge
        src/include/lisa-deskbridge/LisaController.h
        src/core/LisaControllerProxy.cpp
        src/include/lisa-deskbridge/LisaController.h
        src/core/MidiReceiver.cpp
        src/include/lisa-deskbridge/MidiReceiver.h
        src/core/MidiSender.cpp
        src/include/lisa-deskbridge/MidiSender.h
        src/core/MidiClient.cpp
        src/include/lisa-deskbridge/MidiClient.h
        src/core/VirtualMidiDevice.cpp
        src/include/lisa-deskbridge/VirtualMidiDevice.h
        src/core/Bridge.cpp
        src/include/lisa-deskbridge/Bridge.h
        src/core/bridges/SQ6.cpp
        src/include/lisa-deskbridge/bridges/SQ6.h
        src/core/bridges/Generic.cpp
        src/include/lisa-deskbridge/bridges/Generic.h
        src/core/log.cpp
        src/include/lisa-deskbridge/log.h)
target_link_libraries(lisa-deskbridge oscpack libremidi)

add_executable(lisa-deskbridge-cli
        src/core/main.cpp src/core/SQMidiControl.cpp src/include/lisa-deskbridge/SQMidiControl.h)

target_link_libraries(lisa-deskbridge-cli lisa-deskbridge)
