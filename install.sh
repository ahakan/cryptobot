#!/bin/bash

sudo rm -rf build

CMAKE_COMMAND="cmake .. "

# Check libcap
FINDLIBCAP=$(ldconfig -p | grep libcap)

if [ -n "$FINDLIBCAP" ]; then
    echo "-- Found Libcap: TRUE"
    CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_LIBCAP=FALSE"
else
    echo "Libcap will be installed."
    CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_LIBCAP=TRUE"
fi

# Check sqlite
FINDSQLITE=$(ldconfig -p | grep libsqlite3)

if [ -n "$FINDSQLITE" ]; then
    CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_SQLITE3=FALSE"
else
    echo "LibSQLite3 will be installed."
    CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_SQLITE3=TRUE"
fi

# Check sqlite
FINDCURL=$(ldconfig -p | grep curl)

if [ -n "$FINDCURL" ]; then
    CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_CURL=FALSE"
else
    echo "Libcurl will be installed."
    CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_CURL=TRUE"
fi

mkdir build && cd build
${CMAKE_COMMAND}
sudo make