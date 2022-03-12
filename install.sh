#!/bin/bash

[ -d "build/" ] && sudo rm -rf build

NOTFOUND=0

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
CHECKLIBCAP=$(ldconfig -p | grep /lib/libcap)
CHECKLOCALLIBCAP=$(ldconfig -p | grep /local/lib/libcap)

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
CHECKSQLITE=$(ldconfig -p | grep /lib/libsqlite3)
CHECKLOCALSQLITE=$(ldconfig -p | grep /local/lib/libsqlite3)

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
CHECKCURL=$(ldconfig -p | grep /lib/libcurl)
CHECKLOCALCURL=$(ldconfig -p | grep /local/lib/libcurl)

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