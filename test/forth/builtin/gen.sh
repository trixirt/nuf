#!/bin/sh
c=`ls *.c`;
for cc in $c; do
     clang -emit-llvm -S $cc
done
