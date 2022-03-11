#!/bin/bash

sudo rm -rf build

CHECKCMAKE=$(ldconfig -p | cmake --system-information | grep CMAKE_VERSION | cut -c 16-21)

CMAKEREQUIREDVER="3.50.0"

CMAKEURL="https://github.com/Kitware/CMake.git"

if [ "$(printf '%s\n' "$CMAKEREQUIREDVER" "$CHECKCMAKE" | sort -V | head -n1)" = "$CMAKEREQUIREDVER" ]; then 
    git clone $CMAKEURL
    cd CMake
    ./bootstrap && make && sudo make install
else
    echo "-- Found CMAKE: TRUE"
fi

CMAKE_COMMAND="cmake .. "

# Check libcap
CHECKLIBCAP=$(ldconfig -p | grep /usr/lib/libcap)
CHECKLOCALLIBCAP=$(ldconfig -p | grep /usr/local/libcap)

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
    fi
fi

# Check sqlite
CHECKSQLITE=$(ldconfig -p | grep /usr/lib/libsqlite3)
CHECKLOCALSQLITE=$(ldconfig -p | grep /usr/local/libsqlite3)

if [ -n "$CHECKSQLITE" ]; then
    CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_SQLITE3=FALSE"
else
    if [ -n "$CHECKLOCALSQLITE" ]; then
        CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_SQLITE3=FALSE"
    else
        echo "LibSQLite3 will be installed."
        CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_SQLITE3=TRUE"
    fi
fi

# Check sqlite
CHECKCURL=$(ldconfig -p | grep /usr/lib/libcurl)
CHECKLOCALCURL=$(ldconfig -p | grep /usr/local/libcurl)

if [ -n "$CHECKCURL" ]; then
    CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_CURL=FALSE"
else
    if [ -n "$CHECKLOCALCURL" ]; then
        CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_CURL=FALSE"
    else
        echo "Libcurl will be installed."
        CMAKE_COMMAND="${CMAKE_COMMAND} -DBUILD_CURL=TRUE"
    fi
fi

mkdir build && cd build
${CMAKE_COMMAND}
sudo make