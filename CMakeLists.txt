cmake_minimum_required(VERSION 3.24)
project(lisa-deskbridge)

set(CMAKE_CXX_STANDARD 17)

#if(APPLE)
#    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
#endif()

include_directories(src/third_party/oscpack/)
add_subdirectory(src/third_party/oscpack EXCLUDE_FROM_ALL)

add_subdirectory(src/third_party/libremidi EXCLUDE_FROM_ALL)

#include_directories(src/third_party/sqmix-mitm/src)
add_subdirectory(src/third_party/sqmix-mitm EXCLUDE_FROM_ALL)


#include_directories(src/include/lisa-deskbridge)

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
        src/core/bridges/SQMidi.cpp
        src/include/lisa-deskbridge/bridges/SQMidi.h
        src/core/bridges/Generic.cpp
        src/include/lisa-deskbridge/bridges/Generic.h
        src/core/log.cpp
        src/include/lisa-deskbridge/log.h src/core/bridges/SQMitm.cpp src/include/lisa-deskbridge/bridges/SQMitm.h)
target_link_libraries(lisa-deskbridge oscpack libremidi sqmixmitm)
target_include_directories(lisa-deskbridge PUBLIC src/include/)
target_include_directories(lisa-deskbridge PRIVATE src/include/lisa-deskbridge)
#set_target_properties(
#        lisa-deskbridge
#        PROPERTIES
#        ARCHIVE_OUTPUT_DIRECTORY                        "${CMAKE_CURRENT_SOURCE_DIR}/lib"
#)

add_executable(lisa-deskbridge-cli
        src/tools/cli.cpp)

target_link_libraries(lisa-deskbridge-cli lisa-deskbridge)
set_target_properties(
        lisa-deskbridge
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY                        "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

if (APPLE)
#    add_subdirectory(macosx/L-ISA Deskbridge)
    set(APPICON "macosx/L-ISA Deskbridge/assets/AppIcon.icns")
    set_source_files_properties(${APPICON} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(lisa-deskbridge-macosx
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/AppDelegate.h"
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/AppDelegate.m"
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/FindMixerIpViewController.h"
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/FindMixerIpViewController.mm"
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/LogViewController.h"
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/LogViewController.m"
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/Log.h"
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/Log.mm"
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/main.m"
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/MainViewController.h"
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/MainViewController.mm"
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/Extensions.h"
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/Extensions.mm"
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/Settings.h"
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/Settings.mm"
            "macosx/L-ISA Deskbridge/L-ISA Deskbridge/Base.lproj/Main.storyboard"
            ${APPICON}
    )
    target_link_libraries(lisa-deskbridge-macosx PRIVATE lisa-deskbridge "-framework Cocoa")
    set_property(TARGET lisa-deskbridge-macosx APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc")
    target_compile_definitions(lisa-deskbridge-macosx PUBLIC -D_NO_DEMO_)
    set_target_properties(
            lisa-deskbridge-macosx
            PROPERTIES
            MACOSX_BUNDLE                                   "TRUE"
            MACOSX_BUNDLE_INFO_PLIST                        "${CMAKE_CURRENT_SOURCE_DIR}/macosx/L-ISA Deskbridge/cmake/Info.plist.in"

            MACOSX_BUNDLE_GUI_IDENTIFIER                    "tschiemer.lisa-deskbridge.macosx"
            MACOSX_BUNDLE_BUNDLE_NAME                       "L-ISA Deskbridge"
            MACOSX_BUNDLE_BUNDLE_VERSION                    "1.1.0"
            MACOSX_BUNDLE_SHORT_VERSION_STRING              "1.1.0~alpha"
            MACOSX_BUNDLE_LONG_VERSION_STRING               "1.1.0~alpha"
            MACOSX_BUNDLE_COPYRIGHT                         "Copyright (C) 2025 Philip Tschiemer GNU Affero General Public License v3"
            MACOSX_BUNDLE_EXECUTABLE_NAME                   lisa-deskbridge-macosx
            MACOSX_BUNDLE_ICON_FILE                         "AppIcon"
            # MACHO_COMPATIBILITY_VERSION                   "12.0.0"
#            MACOSX_PACKAGE_LOCATION                         "Resources"
#            RESOURCE                                        "${RESOURCE_FILES}"

            XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY              ""
            XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED           NO
            XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT        dwarf
            XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN  YES
            XCODE_ATTRIBUTE_CLANG_LINK_OBJC_RUNTIME         NO

            OUTPUT_NAME                                     "L-ISA Deskbridge"
            RUNTIME_OUTPUT_DIRECTORY                        "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    )

    find_program(ICONUTIL iconutil REQUIRED)
    add_custom_command(
            TARGET lisa-deskbridge-macosx
            PRE_BUILD
            COMMAND cd "${CMAKE_CURRENT_SOURCE_DIR}/macosx/L-ISA Deskbridge/assets" && ${ICONUTIL} -c icns AppIcon.iconset
    )
#    add_custom_command(
#            TARGET lisa-deskbridge-macosx
#            POST_BUILD
#            COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/macosx/L-ISA Deskbridge/assets/AppIcon.icns" "${CMAKE_CURRENT_SOURCE_DIR}/bin/L-ISA Deskbridge.app/Contents/Resources/"
#    )

    find_program(IBTOOL ibtool REQUIRED)
    add_custom_command(
            TARGET lisa-deskbridge-macosx
            POST_BUILD
            COMMAND ${IBTOOL} --compile
                "${CMAKE_CURRENT_SOURCE_DIR}/bin/L-ISA Deskbridge.app/Contents/Resources/Base.lproj/Main.storyboardc"
                "${CMAKE_CURRENT_SOURCE_DIR}/macosx/L-ISA Deskbridge/L-ISA Deskbridge/Base.lproj/Main.storyboard"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/macosx/L-ISA Deskbridge/L-ISA Deskbridge/Base.lproj/Main.storyboard"
            COMMENT "Compiling storyboard file Main.storyboard")
endif()
