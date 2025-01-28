cmake_minimum_required(VERSION 3.24)
project(lisa-deskbridge)

set(CMAKE_CXX_STANDARD 17)


include_directories(src/third_party/oscpack/)
add_subdirectory(src/third_party/oscpack EXCLUDE_FROM_ALL)


add_subdirectory(src/third_party/libremidi EXCLUDE_FROM_ALL)


include_directories(src/include/lisa-deskbridge)

add_executable(lisa-deskbridge src/core/main.cpp src/core/LisaControllerProxy.cpp src/include/lisa-deskbridge/LisaControllerProxy.h src/core/MidiClient.cpp src/include/lisa-deskbridge/MidiClient.h src/core/VirtualMidiDevice.cpp src/include/lisa-deskbridge/VirtualMidiDevice.h src/include/lisa-deskbridge/Midi.h src/core/Bridge.cpp src/include/lisa-deskbridge/Bridge.h src/core/bridges/SQ6.cpp src/include/lisa-deskbridge/bridges/SQ6.h src/core/Midi.cpp)
target_link_libraries(lisa-deskbridge oscpack libremidi)
