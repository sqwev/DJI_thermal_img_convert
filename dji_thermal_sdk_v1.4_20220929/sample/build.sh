#!/bin/bash

# Bash script which run cmake to build sample executable.
#
# @Copyright (c) 2020-2023 DJI. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

PROJ_PATH=$(dirname $(readlink -f "$0"))/
echo "workspace : ${PROJ_PATH}"

BUILD_TYPE_DEBUG=Debug
BUILD_TYPE_RELEASE=Release

BUILD=build/
DIR_DEBUG=${BUILD_TYPE_DEBUG}
DIR_RELEASE=${BUILD_TYPE_RELEASE}
BUILD32=x86
BUILD64=x64

DIR_LIB=${PROJ_PATH}../tsdk-core/lib/linux

echo "****************************************"
echo "Clear build and output folders"
echo "****************************************"

if [ -d "${PROJ_PATH}${BUILD}" ]; then
    echo "Delete build folder"
    rm -rf ${PROJ_PATH}${BUILD}
fi

if [ -d "${PROJ_PATH}${DIR_DEBUG}_${BUILD32}" ]; then
    echo "Delete output folder"
    rm -rf ${PROJ_PATH}${DIR_DEBUG}_${BUILD32}
fi

if [ -d "${PROJ_PATH}${DIR_DEBUG}_${BUILD64}" ]; then
    echo "Delete output folder"
    rm -rf ${PROJ_PATH}${DIR_DEBUG}_${BUILD64}
fi

if [ -d "${PROJ_PATH}${DIR_RELEASE}_${BUILD32}" ]; then
    echo "Delete output folder"
    rm -rf ${PROJ_PATH}${DIR_RELEASE}_${BUILD32}
fi

if [ -d "${PROJ_PATH}${DIR_RELEASE}_${BUILD64}" ]; then
    echo "Delete output folder"
    rm -rf ${PROJ_PATH}${DIR_RELEASE}_${BUILD64}
fi

echo "****************************************"
echo "Create build folder : ${PROJ_PATH}${BUILD}"
echo "****************************************"

mkdir ${PROJ_PATH}${BUILD}

if [ -d "${DIR_LIB}/debug_x86" ]; then
    echo "****************************************"
    echo "Build Configuration ${BUILD_TYPE_DEBUG} ${BUILD32}"
    echo "****************************************"
    mkdir ${PROJ_PATH}${BUILD}${DIR_DEBUG}_${BUILD32}

    cd ${PROJ_PATH}${BUILD}${DIR_DEBUG}_${BUILD32}
    cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE_DEBUG} ../../ -DTARGET_CPU=x86
    cmake --build . --target install --config ${BUILD_TYPE_DEBUG}
fi

if [ -d "${DIR_LIB}/debug_x64" ]; then
    echo "****************************************"
    echo "Build Configuration ${BUILD_TYPE_DEBUG} ${BUILD64}"
    echo "****************************************"
    mkdir ${PROJ_PATH}${BUILD}${DIR_DEBUG}_${BUILD64}

    cd ${PROJ_PATH}${BUILD}${DIR_DEBUG}_${BUILD64}
    cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE_DEBUG} ../../ -DTARGET_CPU=x64
    cmake --build . --target install --config ${BUILD_TYPE_DEBUG}
fi

if [ -d "${DIR_LIB}/release_x86" ]; then
    echo "****************************************"
    echo "Build Configuration ${BUILD_TYPE_RELEASE} ${BUILD32}"
    echo "****************************************"
    mkdir ${PROJ_PATH}${BUILD}${DIR_RELEASE}_${BUILD32}

    cd ${PROJ_PATH}${BUILD}${DIR_RELEASE}_${BUILD32}
    cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE_RELEASE} ../../ -DTARGET_CPU=x86
    cmake --build . --target install --config ${BUILD_TYPE_RELEASE}
fi

if [ -d "${DIR_LIB}/release_x64" ]; then
    echo "****************************************"
    echo "Build Configuration ${BUILD_TYPE_RELEASE} ${BUILD64}"
    echo "****************************************"
    mkdir ${PROJ_PATH}${BUILD}${DIR_RELEASE}_${BUILD64}

    cd ${PROJ_PATH}${BUILD}${DIR_RELEASE}_${BUILD64}
    cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE_RELEASE} ../../ -DTARGET_CPU=x64
    cmake --build . --target install --config ${BUILD_TYPE_RELEASE}
fi
