#!/bin/bash

while getopts b:c: flag
do
    case "${flag}" in
        b) BOOST=${OPTARG};;
        c) CMAKE=${OPTARG};;
    esac
done

[ -d "build/" ] && sudo rm -rf build

NOTFOUND=0

CMAKE_COMMAND="cmake .. "

CMAKEURL="https://github.com/Kitware/CMake.git"

# Install cmake
if [[ "$CMAKE" == 1 ]]; then
    git clone $CMAKEURL
    cd CMake
    ./bootstrap && make && sudo make install
fi

# Check libcap
CHECKLIBCAP=$(ldconfig -p | grep /lib/libcap.so)
CHECKLOCALLIBCAP=$(ldconfig -p | grep /local/lib/libcap.so)

if [ -n "$CHECKLIBCAP" ]; then
    echo "-- Found Libcap: TRUE"
    CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_LIBCAP=FALSE"
else
    if [ -n "$CHECKLOCALLIBCAP" ]; then
        echo "-- Found Libcap: TRUE"
        CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_LIBCAP=FALSE"
    else
        echo "Libcap will be installed."
        CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_LIBCAP=TRUE"
        NOTFOUND=1
    fi
fi

# Check sqlite
CHECKSQLITE=$(ldconfig -p | grep /lib/libsqlite3.so)
CHECKLOCALSQLITE=$(ldconfig -p | grep /local/lib/libsqlite3.so)

if [ -n "$CHECKSQLITE" ]; then
    CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_SQLITE3=FALSE"
else
    if [ -n "$CHECKLOCALSQLITE" ]; then
        CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_SQLITE3=FALSE"
    else
        echo "LibSQLite3 will be installed."
        CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_SQLITE3=TRUE"
        NOTFOUND=1
    fi
fi

# Check sqlite
CHECKCURL=$(ldconfig -p | grep /lib/libcurl.so)
CHECKLOCALCURL=$(ldconfig -p | grep /local/lib/libcurl.so)

if [ -n "$CHECKCURL" ]; then
    CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_CURL=FALSE"
else
    if [ -n "$CHECKLOCALCURL" ]; then
        CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_CURL=FALSE"
    else
        echo "Libcurl will be installed."
        CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_CURL=TRUE"
        NOTFOUND=1
    fi
fi

mkdir build && cd build

${CMAKE_COMMAND}

if [ $NOTFOUND -eq 1 ]; then
    sudo make
else
    make
fi