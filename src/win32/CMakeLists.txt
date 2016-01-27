#this file is part of Casper
# Copyright 2015 CasperDNS Team
# Author CasperDNS App Team <casperdnsteam@gmail.com>

set(APPLICATION_EXE_NAME casperdns)

set(CASPER_WIN_CLIENT_SRC_FILES main.cpp
  ../common/casper_configure_dialog.cpp
  ../common/casper_app_window.cpp
  ../common/service_list.cpp
  ../common/ui_button.cpp
  ../common/casper_runtime.cpp
  ../common/casper_client_win_impl.cpp
)

set(CASPER_WIN_CLIENT_HEADER_FILES
   ../common/casper_configure_dialog.h
   ../common/service_list.h
   ../common/casper_app_window.h
   ../common/ui_button.h
   ../common/casper_runtime.h
   ../common/ui_button.h
   ../common/casper_client.h
)

set(QTMOC_SRCS ${CASPER_WIN_CLIENT_HEADER_FILES})

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(WIN_RESOURCE_FILE ../artwork/windows/app_icon.rc)

set(CMAKE_AUTORCC ON)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src/common/
  ${CMAKE_SOURCE_DIR}/src/casper_launchd/
)

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})
QT5_WRAP_UI(CASPER_WIN_CLIENT_UI_DEFS_FILES
  ${CMAKE_SOURCE_DIR}/src/common/configuredialog.ui
  ${CMAKE_SOURCE_DIR}/src/common/casper_app_window.ui
)

set(CASPER_WIN_CLIENT_SOURCE_FILES
        ${CASPER_WIN_CLIENT_HEADER_FILES}
        ${CASPER_WIN_CLIENT_SRC_FILES}
        ${CASPER_WIN_CLIENT_UI_DEFS_FILES}
	)

if(WIN32)

set(link_libraries
  Dhcpcsvc.lib
  Ws2_32.lib
  taskschd.lib
  comsupp.lib
  wbemuuid 
  d2d1.lib
  d3d11.lib
  dxgi.lib
  windowscodecs.lib
  dwrite.lib
  Iphlpapi.lib
  Qt5::WinMain)

qt5_add_resources(CASPER_WIN_CLIENT_WIDOWS_RESOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/artwork/qt_icon_resource.qrc)

add_executable(${APPLICATION_EXE_NAME} WIN32 ${WIN_APPLICATION}
   ${WIN_RESOURCE_FILE}
   ${CASPER_WIN_CLIENT_WIDOWS_RESOURCE_FILES}
   ${CASPER_WIN_CLIENT_SOURCE_FILES}
   ${CASPER_WIN_CLIENT_HEADER_FILES}
   ${QT_MOC_SRCS}
   )

target_link_libraries(${APPLICATION_EXE_NAME} ${link_libraries})

add_definitions(-DNODEFAULTLIB:library)
set_target_properties(${APPLICATION_EXE_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
set_target_properties(${APPLICATION_EXE_NAME}
  PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" ")
endif()

QT5_USE_MODULES(${APPLICATION_EXE_NAME} Network Widgets Xml Core Gui)

if(WIN32)
  install(TARGETS ${APPLICATION_EXE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
