
cmake_minimum_required(VERSION 3.25)

if( 
    NOT DEFINED SNF_VERSION_MAJOR OR 
    NOT DEFINED SNF_VERSION_MINOR OR
    NOT DEFINED SNF_VERSION_PATCH
    )
    message( FATAL_ERROR "Unset version number run cmake with -DPROJECT_VERSION_{MAJOR|MINOR|PATCH}=X, Note if this is an alpha version then add -DSNF_ALPHA")
endif()



project( snf
    VERSION ${SNF_VERSION_MAJOR}.${SNF_VERSION_MINOR}.${SNF_VERSION_PATCH}
    DESCRIPTION "Shadow Network Framework Library"
    HOMEPAGE_URL "https://docs.abdelhadiseddar.com/snf"
    LANGUAGES C)


if( SNF_ALPHA )
    set(PROJECT_VERSION_TWEAK -alpha)
    set(PROJECT_VERSION ${PROJECT_VERSION}-alpha)
endif()


file(GLOB_RECURSE source_files "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.c")
file(GLOB_RECURSE header_files "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.h")


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin")

include(CMakeShared.txt)
if (
    ${PROJECT_VERSION_MAJOR} STREQUAL 0 
    AND
    DEFINED ${SNF_ALPHA}
)
    set_target_properties(
        snf 
        PROPERTIES 
            VERSION     ${PROJECT_VERSION})
else()
    set_target_properties(
        snf 
        PROPERTIES 
            VERSION     ${PROJECT_VERSION_MAJOR}
            SOVERSION   ${PROJECT_VERSION})
endif()

target_sources(
    snf 
    PUBLIC 
    FILE_SET HEADERS
        BASE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/../src/
        FILES 
            ${header_files})

target_link_libraries(snf
    uuid
    m
    pthread
)

install(
    TARGETS snf ${TESTS}
    LIBRARY
        COMPONENT Release
        NAMELINK_COMPONENT Release
    FILE_SET HEADERS
       COMPONENT Release
)

include(
    CMakeTests.txt
)

if( 
    ${CMAKE_SYSTEM_PROCESSOR} STREQUAL x86_64 
    OR 
    ${CMAKE_SYSTEM_PROCESSOR} STREQUAL amd64
    )
    set(CPU_TYPE amd64)
else()
    set(CPU_TYPE ${CMAKE_SYSTEM_PROCESSOR})
endif()
set(CPACK_PACKAGE_CONTACT "Abdelhadi Seddar <contact@abdelhadiseddar.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Shadow Network Framework's Server Side Library!")
set(CPACK_PACKAGE_VENDOR "Abdelhadi Seddar")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../packages")
#
# Binary Packages
#
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}_${PROJECT_VERSION}_${CMAKE_SYSTEM_NAME}_${CPU_TYPE}")

if( PACKAGE_BIN_DEBIAN )
    set(CPACK_BINARY_DEB "ON")
    set(CPACK_DEBIAN_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_DEBIAN_PACKAGE_RELEASE  0)

    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6), uuid-dev (>= 2.38)")
    set(CPACK_DEBIAN_PACKAGE_NAME "snf")
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
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}_${PROJECT_VERSION}")

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