cmake_minimum_required(VERSION 3.10)
project(sample)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    set(CONAN_SYSTEM_INCLUDES ON)
    conan_basic_setup()
else()
    message(FATAL_ERROR "Missing conanbuildinfo.cmake, run conan install first")
endif()

set(SOURCES
    src/main.cpp
    src/portaudio/PaAudioInputAdapter.cpp
    src/portaudio/PaAudioInputAdapterFactory.cpp
)
set(HEADERS
    src/portaudio/PaAudioInputAdapter.hpp
    src/portaudio/PaAudioInputAdapterFactory.hpp
    src/portaudio/Utils.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
conan_target_link_libraries(${PROJECT_NAME})
if(WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32 wsock32) # Needed by Boost.Asio
elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PortAudio REQUIRED IMPORTED_TARGET portaudio-2.0)
    target_link_libraries(${PROJECT_NAME} PkgConfig::PortAudio)
endif()




target_link_libraries(sample paho-mqttpp3 paho-mqtt3as)