#!/bin/bash

#This script take care of creating the .scop files from all the .c
#files in a directory.

print_usage() {
    echo "Usage : $0 <sources_directory> <scops_directory>"
}

# Check if there is enough argument
if [ $# -lt 2 ] ; then
    echo "ERROR : Not enough arguments."
    print_usage $0
    exit
fi

# Check if the directories given as arguments are directories and exists.
if ! [ -d $1 ] ; then
    echo "ERROR : $1 is not a directory or doesn't exists."
    print_usage $0
    exit
fi
if ! [ -d $2 ] ; then
    echo "ERROR : $2 is not a directory or doesn't exists."
    print_usage $0
    exit
fi

# Check if clan is installed
if ! type clan >/dev/null 2>&1  ; then
    echo "This script require "clan" but it's not installed.  Aborting.";
    exit 1;
fi

for src in $(ls $1/*.c)
do
    clan $src > $2/${src/$1//}.scop
done
