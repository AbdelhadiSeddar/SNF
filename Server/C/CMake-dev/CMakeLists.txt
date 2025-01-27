
cmake_minimum_required(VERSION 3.25)

if( 
    NOT DEFINED SNF_VERSION_MAJOR OR 
    NOT DEFINED SNF_VERSION_MINOR OR
    NOT DEFINED SNF_VERSION_PATCH
    )
  message( FATAL_ERROR "Unset version number run cmake with -DSNF_VERSION_{MAJOR|MINOR|PATCH}=X.\n* Note if this is an alpha version then add -DSNF_ALPHA")
endif()


project( snf-dev
    VERSION ${SNF_VERSION_MAJOR}.${SNF_VERSION_MINOR}.${SNF_VERSION_PATCH}
    DESCRIPTION "Shadow Network Framework Library Headers"
    HOMEPAGE_URL "https://docs.abdelhadiseddar.com/snf"
    LANGUAGES C)


if( SNF_ALPHA )
    set(PROJECT_VERSION_TWEAK -alpha)
    set(PROJECT_VERSION ${PROJECT_VERSION}-alpha)
    set(ALPHA_V -alpha)
endif()




add_library(snf-dev INTERFACE)

get_filename_component(
  HEADERS_PATH 
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/" 
  ABSOLUTE
)

target_include_directories(
    snf-dev
    INTERFACE 
    "${HEADERS_PATH}"
)

file(GLOB_RECURSE header_files "${HEADERS_PATH}/*.h")

############################################################################################
##
## Section: Preparing and staging.
##
## Preparing staging Directory
set(
  STAGING_DIR
  "/usr/include"
)
## 
##
## Prepare Headers
install(
    DIRECTORY  "${HEADERS_PATH}"
    DESTINATION ${STAGING_DIR}
    FILES_MATCHING PATTERN "*.h"
)
##
############################################################################################
############################################################################################

if( 
    ${CMAKE_SYSTEM_PROCESSOR} STREQUAL x86_64 
    OR 
    ${CMAKE_SYSTEM_PROCESSOR} STREQUAL amd64
    )
    set(CPU_TYPE x86-64)
else()
    set(CPU_TYPE ${CMAKE_SYSTEM_PROCESSOR})
endif()
set(CPACK_PACKAGE_CONTACT "Abdelhadi Seddar <contact@abdelhadiseddar.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Shadow Network Framework's Server Side Library's header files!")
set(CPACK_PACKAGE_VENDOR "Abdelhadi Seddar")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../packages")

string(TOLOWER "${CMAKE_SYSTEM_NAME}" SYS)
#
# Binary Packages
#

set(CPACK_PACKAGE_FILE_NAME "lib${PROJECT_NAME}_${PROJECT_VERSION}")

if( "$ENV{PACKAGE_BIN_DEBIAN}" STREQUAL true )
    set(CPACK_BINARY_DEB "ON")
    set(CPACK_DEBIAN_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_DEBIAN_PACKAGE_RELEASE  0)
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS "OFF")

    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsnf (>= 0.0.3-alpha)")
    set(CPACK_DEBIAN_PACKAGE_NAME "libsnf-dev")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://docs.abdelhadiseddar.com/snf")

else()
    set(CPACK_BINARY_DEB "OFF")
endif()

set(CPACK_BINARY_FREEBSD "OFF")
set(CPACK_BINARY_IFW "OFF")
set(CPACK_BINARY_NSIS "OFF")
set(CPACK_BINARY_RPM "OFF")
set(CPACK_BINARY_STGZ "OFF")
set(CPACK_BINARY_TBZ2 "OFF")
set(CPACK_BINARY_TXZ "OFF")
set(CPACK_BINARY_TGZ "OFF")
set(CPACK_BINARY_TZ "OFF")



#
#   Source Packages
#
set(CPACK_SOURCE_PACKAGE_FILE_NAME "lib${PROJECT_NAME}-src_${PROJECT_VERSION}")

if( PACKAGE_SRC_TGZ )
    set(CPACK_SOURCE_TGZ "ON")
else()
    set(CPACK_SOURCE_TGZ "OFF")
endif()

if( PACKAGE_SRC_ZIP )
    set(CPACK_SOURCE_ZIP "ON")
else()
    set(CPACK_SOURCE_ZIP "OFF")
endif()

set(CPACK_SOURCE_RPM "OFF")
set(CPACK_SOURCE_TBZ2 "OFF")
set(CPACK_SOURCE_TXZ "OFF")
set(CPACK_SOURCE_TZ "OFF")

include(CPack)
