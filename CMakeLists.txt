cmake_minimum_required(VERSION 3.13)
project(daemon_process VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
set(PAHO_CPP_LIB paho-mqttpp3)
set(PAHO_C_LIB paho-mqtt3a)

find_package( OpenCV REQUIRED )
add_executable(daemon_process main.cpp doorLock.cpp doorLock.h camera.cpp camera.h terminal.cpp terminal.h logger.cpp logger.h module.cpp module.h debug.h sound_manager.cpp sound_manager.h vendor_storage.cpp vendor_storage.h)
target_link_libraries( daemon_process ${OpenCV_LIBS} ${PAHO_C_LIB} ${PAHO_CPP_LIB} pthread curl mysqlclient stdc++fs asound)

add_definitions(-DOPENSSL)

message("-- Opencv version ${OpenCV_VERSION}")
message("-- Project major version ${CMAKE_PROJECT_VERSION_MAJOR}")
message("-- Project minor version ${CMAKE_PROJECT_VERSION_MINOR}")
message("-- Project patch version ${CMAKE_PROJECT_VERSION_PATCH}")
add_definitions(-DPROJECT_VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR})
add_definitions(-DPROJECT_VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR})
add_definitions(-DPROJECT_VERSION_PATCH=${CMAKE_PROJECT_VERSION_PATCH})
if(OpenCV_VERSION VERSION_LESS "4")
    message("-- Add OLD_OPENCV flag")
    add_definitions(-DOLD_OPENCV)
endif()
