#!/bin/bash


res_files_directory=""
graphes_directory=""


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
    n=$(grep -A1 "#rate" $f | tail -n 1 | cut -d " " -f3 | cut -d "	" -f2)
    f=$(basename $f)
    gnuplot <<- EOF
set xlabel "Minimal rating to aggregate statements"
set ylabel "Pourcentage of aggregated statements"
set yrange [0:100]
set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.85
set title "SuBStrAte aggregation results for ${f/\.c\.*/\.c}\nOriginal number of statement : $n"
set term pngcairo size 960,720
set output "${graphes_directory}/${f}.png"
plot "${res_files_directory}/${f}" using ((\$3 == 0) ? 0.5 : \$3*100.0 / \$2):xticlabels(1) title "Parallelization", \
    "${res_files_directory}/${f}" using ((\$4 == 0) ? 0.5 : \$4*100.0 / \$2) title "Reuse", \
    "${res_files_directory}/${f}" using ((\$5 == 0) ? 0.5 : \$5*100.0 / \$2) title "Tiling Hyperplane", \
    "${res_files_directory}/${f}" using ((\$6 == 0) ? 0.5 : \$6*100.0 / \$2) title "Vectorization"
EOF
done

