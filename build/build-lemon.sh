#!/bin/sh

FILENAME=`basename $0`
BASENAME=${FILENAME%.*}
HERE=`realpath $(dirname $0)`

cd $HERE
install=$HERE/../tools/bin

if [ -f $install/lemon ]; then
    rm $install/lemon
fi
mkdir -p $install
cc $HERE/../tools/src/lemon/lemon.c -o $install/lemon

