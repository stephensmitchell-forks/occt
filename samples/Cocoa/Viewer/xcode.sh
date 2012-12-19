#!/bin/bash

pushd .
source ../../../env.sh "$1"
popd

open /Developer/Applications/Xcode.app Viewer.xcodeproj
