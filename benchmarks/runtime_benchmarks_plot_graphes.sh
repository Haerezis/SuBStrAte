#!/bin/bash


res_files_directory=""
graphes_directory=""
EVERY=0

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
set xlabel "Minimal rating to aggregate statements"
set ylabel "Speedup in pourcentage"
set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.85
set title "Runtime speedup (vs. Pluto) for ${f/\.c\.*/\.c}\n"
set term pngcairo size 960,720
set output "${graphes_directory}/${f}.png"
plot "${res_files_directory}/${f}" using 2:xticlabels(1) title "Parallelization" every :::$EVERY::$EVERY, \
    "${res_files_directory}/${f}" using 3 title "Reuse" every :::$EVERY::$EVERY, \
    "${res_files_directory}/${f}" using 4 title "Tiling Hyperplane" every :::$EVERY::$EVERY, \
    "${res_files_directory}/${f}" using 5 title "Vectorization" every :::$EVERY::$EVERY
EOF
done

