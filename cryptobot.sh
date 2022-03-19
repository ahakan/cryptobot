#!/bin/bash

PATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

BUILDPATH="$PATH/build"

cd $BUILDPATH

./cryptobot
