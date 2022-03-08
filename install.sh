#!/bin/bash

CMAKE_COMMAND="cmake .. "

# Check libcap
FINDLIBCAP=$(ldconfig -p | grep libcap)

if [ -n "$FINDLIBCAP" ]; then
    echo $FINDLIBCAP
    CMAKE_COMMAND="${CMAKE_COMMAND} -DLIBCAP_BUILD:STRING=NO"
else
    echo "Libcap will be installed."
    CMAKE_COMMAND="${CMAKE_COMMAND} -DLIBCAP_BUILD:STRING=YES"
fi

# Check sqlite
FINDSQLITE=$(ldconfig -p | grep libsqlite)

if [ -n "$FINDSQLITE" ]; then
    echo $FINDSQLITE
    CMAKE_COMMAND="${CMAKE_COMMAND} -DLIBSQLITE_BUILD:STRING=NO"
else
    echo "Libsqlite will be installed."
    CMAKE_COMMAND="${CMAKE_COMMAND} -DLIBSQLITE_BUILD:STRING=YES"
fi

sudo rm -rf build
mkdir build && cd build
${CMAKE_COMMAND}
sudo make