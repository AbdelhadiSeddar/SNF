#!/bin/bash
PROJECT="SNF"
lib_name="snf"

lib_v_MAJOR="0"
lib_v_MINOR="0"
lib_v_PATCH="3"
lib_v_ALPHA=true

lib_v=$lib_v_MAJOR"."$lib_v_MINOR"."$lib_v_PATCH

if [[ $lib_v_ALPHA == true ]]; then
  lib_v=$lib_v"-alpha";
fi;


BIN="./bin"
SRC="./src"
CMAKE="./CMake"
CMD=""

clean=false
clean_cmake_all=false

build=true
build_force=false
build_deb=false

install=false
test=false
test_subject=""


package_binary=false
package_formats=()
dev=false

i=0
for ((i=1;i<=$#;i++)); 
do
    if [[ ${!i} = "-d" ]] || [[ ${!i} = "--dev" ]]
    then
        dev=true
    elif [[ ${!i} = "-c" ]] || [[ ${!i} = "--clean" ]] 
    then
        clean=true    
    elif [[ ${!i} = "-cc" ]] || [[ ${!i} = "--clean-cmake" ]] 
    then
        clean_cmake_all=true
    elif [[ ${!i} = "-i" ]] || [[ ${!i} = "--install" ]] 
    then
        install=true
    elif [[ ${!i} = "-nb" ]] || [[ ${!i} = "--no-build" ]] 
    then
        build=false
    elif [[ ${!i} = "-fb" ]] || [[ ${!i} = "--force-build" ]] 
    then
        build_force=true
    elif [[ ${!i} = "-t" ]] || [[ ${!i} = "--test" ]]
    then
        if [[ -z "${TESTING}" ]]
        then
          echo No More than 1 test allowed.
        fi

        ((i++))
        if [[ $# -lt $i ]]
        then
            export TESTING=false
            echo No Test was specified
            exit
        fi
        export TESTING=true
        if [[ ${!i} = "main" ]] || [[ ${!i} = "m" ]] || [[ ${!i} = "Main" ]]
        then
            export TESTING_SUBJECT=Main
        else
            export TESTING_SUBJECT=${!i}
        fi
        # TODO Make a check for test files
        # TODO: Add a way for test files
    elif [[ ${!i} = "-p" ]] || [[ ${!i} = "--package" ]] 
    then 
        ((i++))
        if [[ $# -lt $i ]]
        then
            echo No Package Format was specified
            exit
        fi
        package_binary=true
        if [[ ${!i} = "d" ]] || [[ ${!i} = "deb" ]] || [[ ${!i} = "debian" ]]
        then
            export PACKAGE_BIN_DEBIAN=true
        else
            echo Package Format \"${!i}\" is not supported, exiting.
            exit
        fi
    elif [[ ${!i} = "-ps" ]] || [[ ${!i} = "--package-source" ]] 
    then 
        ((i++))
        if [[ $# -lt $i ]]
        then
            echo No Package Source Format was specified
            exit
        fi
        package_source=true
        if [[ ${!i} = "tgz" ]] || [[ ${!i} = "TGZ" ]]
        then
            package_source_formats+=("TGZ")
        elif [[ ${!i} = "zip" ]] || [[ ${!i} = "ZIP" ]]
        then
            package_source_formats+=("ZIP")
        else
            echo Package Source Format \"${!i}\" is not supported, exiting.
            exit
        fi
    
    elif [[ ${!i} = "-v" ]] || [[ ${!i} = "--version" ]] 
    then
        echo "-- Builder for lib"$lib_name" version "$lib_v
        exit
    elif [[ ${!i} = "-h" ]] ||  [[ ${!i} = "--help" ]]
    then
        echo "-- Builder for lib"$lib_name" version "$lib_v
        echo "-- Format: build.sh [<flag1> <flag2> ...]"
        echo Available Flags:
        echo -e [1]'\t'\-h'\t'--help'\t''\t''\t'Shows this help menu
        echo -e [2]'\t'\-c'\t'--clean'\t''\t''\t'Cleans the bin folder
        echo -e [3]'\t'\-cc'\t'--clean-cmake'\t''\t'Cleans the bin folder and everything generated by CMake
        echo -e [4]'\t'\-t'\t'--test'\t''\t'\<Test\>'\t'Compiles and Executes \<Test\>.c that\'s situated in ./tests/
        echo -e '\t''\t''\t''\t'Main'\t'Compiles and Executes main.c that is considered the Main Test
        echo -e '\t''\t''\t''\t''\t''\t'* \[Inactive with flag "--dev"\]
        echo -e [5]'\t'\-nb'\t'--no-build'\t''\t'Blocks Compilation of binaries
        echo -e '\t''\t''\t''\t''\t''\t'* \[Inactive with flag "--dev"\]
        echo -e [6]'\t'\-fb'\t'--force-build'\t''\t'Forces the \(re\)compilation of binaries even if cashed
        echo -e '\t''\t''\t''\t''\t''\t'* \[Inactive with flag "--dev"\]
        echo -e [7]'\t'\-v'\t'--version'\t''\t'Shows this library\'s version
        echo -e [7]'\t'\-d'\t'--dev '\t''\t''\t'Creates Developper packages\(Headers\)
        echo
        
        exit
    fi

done;

cmk_info_flags=" -DSNF_VERSION_MAJOR="$lib_v_MAJOR" -DSNF_VERSION_MINOR="$lib_v_MINOR" -DSNF_VERSION_PATCH="$lib_v_PATCH
if [[ $lib_v_ALPHA = true ]]
then
    cmk_info_flags+=" -DSNF_ALPHA=true "
fi 

export MODE_DEV=$dev

function cmake_v
{
    cmake $cmk_info_flags $@
}
#####################################
#                                   #
#   Cleaning                        #
#                                   #
#####################################

if [[ $clean = true ]] || [[ $clean_cmake_all = true ]]
then
    if [[ $dev = false ]] && [[ -d $CMAKE ]]
    then
        cd $CMAKE
        if [[ -f Makefile ]]
        then
            if [[ $clean = true ]]
            then
                make  clean
            fi
        fi
        if [[ $clean_cmake_all = true ]]
        then
            rm *.cmake
            rm CMakeCache.txt Makefile CPack*
            rm snf*

            rm -rf CMakeFiles CMake _CPack*
        fi
        cd ..
    fi
    if [[ $dev = true ]] && [[ -d "$CMAKE""-dev" ]]
    then
        cd $CMAKE"-dev"
        if [[ -f Makefile ]]
        then
            if [[ $clean = true ]]
            then
                make  clean
            fi
        fi
        if [[ $clean_cmake_all = true ]]
        then
            rm *.cmake
            rm CMakeCache.txt Makefile CPack*
            rm snf*

            rm -rf CMakeFiles CMake _CPack*
        fi
        cd ..
    fi
    rm ../packages -rf
    if [[ -d $BIN ]]
    then
        rm -rf $BIN/*
    else
        mkdir $BIN
    fi
    echo "-- Cleaned "
    if [[ $build = false ]]
    then
        exit
    fi    
fi

#############################################
#                                           #
#   Checking Folders                        #
#                                           #
#############################################
if [[ ! -d $CMAKE ]] ||  [[ ! -d $SRC ]]; then
    echo "Either CMake or Source folder is inexistant";
fi;


#############################################
#                                           #
#   Executing Commands                      #
#                                           #
#############################################
function CheckBuild
{
    if [[ ! -f ../$BIN"/libsnf.so.$lib_v" ]]
    then
        echo -- Error: build failed!!!
        exit;
    fi
}


function Bin_Make
{
cd $CMAKE
cmake_v ./
if [[ $build_force = true ]]
then
    make clean
fi
if [[ $build = true ]]
then 
    make snf
    CheckBuild
    echo -- Build Successful
fi
if [[ $install = true ]]
then
    CheckBuild
    make install
fi

if [[ ${TESTING} = true ]] 
then
    CheckBuild
    make Test_${TESTING_SUBJECT}
    ../$BIN/Test_${TESTING_SUBJECT}
fi

if [[ $package_binary = true ]]
then
    CheckBuild
    make package
fi

#   TODO: Fix CPack to include proper source code instead of includeing Just CMake/ 
#if [[ $package_source = true ]]
#then
#    cmake_packages=""
#    for str in ${package_source_formats[@]}
#    do
#        cmake_packages+=" -DPACKAGE_SRC_$str=true "
#    done
#    cmake_v $(pwd) $cmake_packages
#
#    make package_source
#
#fi
cd ..
}


function Dev_Make
{
cd "$CMAKE""-dev"
cmake_v ./
if [[ $install = true ]]
then
    CheckBuild
    make install
fi

if [[ $package_binary = true ]]
then
    CheckBuild
    make package
fi

#   TODO: Fix CPack to include proper source code instead of includeing Just CMake/ 
#if [[ $package_source = true ]]
#then
#    cmake_packages=""
#    for str in ${package_source_formats[@]}
#    do
#        cmake_packages+=" -DPACKAGE_SRC_$str=true "
#    done
#    cmake_v $(pwd) $cmake_packages
#
#    make package_source
#
#fi
cd ..
}


if [[ $dev = false ]]
then
  Bin_Make
else
  Dev_Make
fi
