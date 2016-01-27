# this file is part of Casper
# Copyright 2015 CasperDNS Team
# Author CasperDNS App Team <casperdnsteam@gmail.com>

project(CasperDNS)
cmake_minimum_required(VERSION 3.2.0)

cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0003 OLD)
cmake_policy(SET CMP0043 NEW)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_STANDARD 11)

set(QT_REQUIRED_VERSION 5.4.0)

find_package(Qt5 ${QT_REQUIRED_VERSION} REQUIRED
      Core
      Widgets
      Gui
      Svg
      Xml)

include_directories (
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR})

if(MSVC AND MSVC_VERSION GREATER 1500)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
endif(MSVC AND MSVC_VERSION GREATER 1500)

if(WIN32)
  if (ENABLE_BUILD_OPTIMIZATIONS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /02")
  endif()

  if (ENABLE_BUILD_32BIT_OPTIMIZATIONS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /DNOMINMAX")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX")
  endif()
endif()

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include_directories(
    ${CMAKE_BINARY_DIR}/src/win32/
    )

if (WIN32)
 add_subdirectory(src/win32)
endif()
