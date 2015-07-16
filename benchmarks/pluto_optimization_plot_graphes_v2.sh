#!/bin/bash

#Script to use with speedup results per nb_stmt, and not per minimal aggregating rate.

res_files_directory=""
graphes_directory=""
block_index=0

print_usage() {
    echo "Usage : $0 <results_files_directory> <output_graphes_directory>"
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
# Check if the directories given as arguments are directories and exists.
if ! [ -d $2 ] ; then
    echo "ERROR : $2 is not a directory or doesn't exists."
    print_usage $0
    exit
fi

res_files_directory=$1
graphes_directory=$2

gnuplot_style=bar
for f in $(ls ${res_files_directory}/*.results) ; do
    f=$(basename $f)
    gnuplot <<- EOF
set xlabel "Number of aggregated statements"
set ylabel "Average speedup"
set yrange [*:2]
set datafile separator ","
set style data histogram
set style histogram cluster gap 1
set style histogram errorbars gap 1 lw 1
set style fill solid border -1
set boxwidth 0.85
set title "Pluto runtime speedup for ${f/\.c\.*/\.c}\n"
set term pngcairo size 960,720
set output "${graphes_directory}/${f}.png"
plot "${res_files_directory}/${f}" i $block_index using 2:3:xticlabels(1) title "Parallelization", \
    "${res_files_directory}/${f}"  i $block_index using 4:5:xticlabels(1) title "Reuse", \
    "${res_files_directory}/${f}"  i $block_index using 6:7:xticlabels(1) title "Tiling Hyperplane", \
    "${res_files_directory}/${f}"  i $block_index using 8:9:xticlabels(1) title "Vectorization"
EOF
done

