#!/bin/sh

for a in `cat allsc.txt`
do
  echo "Processing $a"
  perl prep.pl < $a.lua > pscripts/$a.lua
done
