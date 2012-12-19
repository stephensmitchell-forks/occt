#!/bin/bash

pushd .
source ../../../env.sh "$1"
popd

open ./build/Debug/Viewer.app
