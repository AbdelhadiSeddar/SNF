cmake_minimum_required(VERSION 3.25)
############################################################################################
##
##  Section: Checks and Initial Setup
##
## Check if version is inputted correctly
if( 
    NOT DEFINED SNF_VERSION_MAJOR OR 
    NOT DEFINED SNF_VERSION_MINOR OR
    NOT DEFINED SNF_VERSION_PATCH
    )
  message( FATAL_ERROR "Unset version number run cmake with -DSNF_VERSION_{MAJOR|MINOR|PATCH}=X.\n* Note if this is an alpha version then add -DSNF_ALPHA")
endif()
##
## Project's Values
project( snf
    VERSION ${SNF_VERSION_MAJOR}.${SNF_VERSION_MINOR}.${SNF_VERSION_PATCH}
    DESCRIPTION "Shadow Network Framework Library"
    HOMEPAGE_URL "https://docs.abdelhadiseddar.com/snf"
    LANGUAGES C)
##
## Extra steps for alpha
if( SNF_ALPHA )
    set(PROJECT_VERSION_TWEAK -alpha)
    set(PROJECT_VERSION ${PROJECT_VERSION}-alpha)
    set(ALPHA_V -alpha)
endif()
##
## Add Necessary .c and .h for compilation.
file(GLOB_RECURSE source_files "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.c")
file(GLOB_RECURSE header_files "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.h")
## 
## Setting Necessary paths
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin")
##
## Include Setup for shared Library
include(CMakeShared.txt)
##
## Set the Target Library's FULL name
set_target_properties(
  snf 
  PROPERTIES 
    SUFFIX      ".so.${PROJECT_VERSION}"
  )
##
## Add the headers needed for compilation
target_sources(
    snf 
    PUBLIC 
    FILE_SET HEADERS
        BASE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/../src/
        FILES 
             ${header_files}
)
##
## Added Llinking to necessary libraries
target_link_libraries(snf
    uuid
    m
    pthread
)
##
## Included a CMake File that sets up Tests
include(
    CMakeTests.txt
)
##
############################################################################################

############################################################################################
##
## Section: Preparing and staging.
##
## Preparing staging Directory
set(
  STAGING_DIR
  "/usr/lib"
)
## 
##
## Prepare Main Binary
install(
    TARGETS snf
    LIBRARY
    COMPONENT Release
    NAMELINK_COMPONENT Release
    DESTINATION ${STAGING_DIR}
)
##
## Check if Testing is intended
if( "$ENV{TESTING}" STREQUAL true)
install(
  TARGETS Test_$ENV{TESTING_SUBJECT}
  COMPONENT Release
)
endif()
##
############################################################################################

############################################################################################
##
##  Section: Create Symlinks after building manually.
##
## SYMLINK Creation Function
function(create_ver_symlink SRC DES)
get_filename_component(
  TARG 
  "${SRC}" 
  ABSOLUTE
)
get_filename_component(
  DEST 
  "${DES}" 
  NAME
)
add_custom_command(
  TARGET snf
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} 
    -E create_symlink 
      ${SRC}
      ${DEST}
  BYPRODUCTS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${DEST}
  WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
  VERBATIM
)
install(
  FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${DEST}"
  COMPONENT Release
  DESTINATION ${STAGING_DIR}
)
endfunction()
##
##
##  SYMLINK:
##    FROM: lib<PROJECT_NAME>.so.<COMPLETE_VERSION_VALUE>
##    TO:   lib<PROJECT_NAME>.so.<VERSION_MAJOR>.<VERSION_MINOR><VERSION_TWEAK if exists>
create_ver_symlink(
  "lib${PROJECT_NAME}.so.${PROJECT_VERSION}"
  "lib${PROJECT_NAME}.so.${SNF_VERSION_MAJOR}.${SNF_VERSION_MINOR}${ALPHA_V}"
)
## 
##  SYMLINK:
##    FROM:   lib<PROJECT_NAME>.so.<VERSION_MAJOR>.<VERSION_MINOR><VERSION_TWEAK if exists>
##    TO:   lib<PROJECT_NAME>.so.<VERSION_MAJOR>.<VERSION_TWEAK if exists>
create_ver_symlink(
  "lib${PROJECT_NAME}.so.${SNF_VERSION_MAJOR}.${SNF_VERSION_MINOR}${ALPHA_V}"
  "lib${PROJECT_NAME}.so.${SNF_VERSION_MAJOR}${ALPHA_V}"
)  
##
##  SYMLINK:
##    FROM:   lib<PROJECT_NAME>.so.<VERSION_MAJOR>.<VERSION_MINOR><VERSION_TWEAK if exists>
##    TO:   lib<PROJECT_NAME>.so.<VERSION_MAJOR>
create_ver_symlink(
  "lib${PROJECT_NAME}.so.${SNF_VERSION_MAJOR}${ALPHA_V}"
  "lib${PROJECT_NAME}.so"
)
##
############################################################################################


############################################################################################
##
##  Section: Packaging
##
## Preparing Variables
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
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Shadow Network Framework's Server Side Library!")
set(CPACK_PACKAGE_VENDOR "Abdelhadi Seddar")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../packages")
string(TOLOWER "${CMAKE_SYSTEM_NAME}" SYS)
set(CPACK_PACKAGE_FILE_NAME "lib${PROJECT_NAME}_${PROJECT_VERSION}_${SYS}_${CPU_TYPE}")
##
## Binary Packages
##
## Debian Package
if( "$ENV{PACKAGE_BIN_DEBIAN}" STREQUAL true )
    set(CPACK_BINARY_DEB "ON")
    set(CPACK_DEBIAN_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_DEBIAN_PACKAGE_RELEASE  0)
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS "OFF")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6), libuuid1 (>= 2.38)")
    set(CPACK_DEBIAN_PACKAGE_NAME "libsnf")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://docs.abdelhadiseddar.com/snf")
else()
    set(CPACK_BINARY_DEB "OFF")
endif()
##
## The Rest are Disabled for now
set(CPACK_BINARY_FREEBSD "OFF")
set(CPACK_BINARY_IFW "OFF")
set(CPACK_BINARY_NSIS "OFF")
set(CPACK_BINARY_RPM "OFF")
set(CPACK_BINARY_STGZ "OFF")
set(CPACK_BINARY_TBZ2 "OFF")
set(CPACK_BINARY_TXZ "OFF")
set(CPACK_BINARY_TGZ "OFF")
set(CPACK_BINARY_TZ "OFF")
##
## Include CPack for ability to make packages.
include(CPack)
##
############################################################################################
