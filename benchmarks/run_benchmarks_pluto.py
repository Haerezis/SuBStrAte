#!/usr/bin/python3

import os, os.path, glob, re
from sys import argv
import time
from statistics import *
from subprocess import call

devnull = open(os.devnull, 'w')


write_res_directly=False
nb_run=5
pluto_output_filename="/tmp/pluto_output.c"
bin_output_filename="/tmp/a.out"
gcc_flags=["-lm","-fopenmp",]# "-DEXTRALARGE_DATASET"]
pluto_options=["--parallel",
        #"--tile",
        ]

#Gnuplot command printed in the data file (to be able to directly
#plot the data
gnuplot_commands = "#Gnuplot datafile"

#Gnuplot comment printed before the data, explaining the format
gnuplot_format = """\
# Format : min_rate_for_aggregation nb_aggregated_statement_TYPE.... nb_statement_original_scop
# with TYPE being 'reuse', 'parallelism', 'vectorization' etc..."""


def benchmark(source_filepath, scop_filepath) :
    res = []
    polycc_cmd=[polycc_path,
            "--readscop",
            scop_filepath,
            "-o",
            pluto_output_filename]
    polycc_cmd=polycc_cmd+pluto_options
    if call(polycc_cmd, stdout=devnull, stderr=devnull) is not 0 :
        print(polycc_cmd)
        raise Exception("Pluto call failed")
    inscop_cmd=[
            inscop_path,
            source_filepath,
            pluto_output_filename,
            pluto_output_filename]
    if call(inscop_cmd, stdout=devnull) is not 0 :
        print(inscop_cmd)
        raise Exception("inscop call failed")
    gcc_cmd = ["gcc",
            "-o",
            bin_output_filename,
            pluto_output_filename,
            "{0}/polybench.c".format(sources_dir),
            "-I",
            sources_dir]
    gcc_cmd=gcc_cmd+gcc_flags
    if call(gcc_cmd, stdout=devnull) is not 0 :
        print(gcc_cmd)
        raise Exception("gcc call failed")

    for run in range(0,nb_run) :
        before=time.time()
        call([bin_output_filename], stdout=devnull)
        after=time.time()
        res.append(after - before)

    return mean(res)

def gnuplot_data(results, source_filename, type_list, rate_list) :
    pluto_res=results["pluto"]
    res_filepath = "{0}/{1}.results".format(res_dir, source_filename)
    with open(res_filepath, "w") as output :
        output.write(gnuplot_commands+"\n\n")
        output.write(gnuplot_format+'\n')
        
        output.write("#")
        output.write("rate")
        #output.write("\tpluto")
        for k in sorted(type_list) : output.write(" \t"+k)
        output.write('\n')
        for rate in rate_list :
            output.write(" {0}".format(rate,pluto_res))
            #output.write(" \t%.3f" % (pluto_res))
            for _type in type_list :
                substrate_res=results[rate][_type]
                substrate_res=100 * (pluto_res - substrate_res) / pluto_res
                output.write("\t"+('%.3f' % substrate_res))
            output.write("\n")


def print_usage() :
    print("Usage : {0} <polycc_path> <inscop_path> <sources_directory> <original_scops_directory> <aggregated_scops_directory> <results_directory> [--write-res]"\
            .format(argv[0]))



# Check if there is enough argument
if len(argv) < 7 :
    print("ERROR : Not enough arguments.")
    print_usage()
    exit(1)

polycc_path=argv[1]
inscop_path=argv[2]
sources_dir=argv[3]
scop_dir=argv[4]
aggr_scops_dir=argv[5]
res_dir=argv[6]

if not os.path.isfile(polycc_path) :
    print("ERROR : polycc program \"{0}\" doesn't exists.".format(substrate_bin))
    print_usage()
    exit(1)
if not os.path.isfile(inscop_path) :
    print("ERROR : inscop program \"{0}\" doesn't exists.".format(substrate_bin))
    print_usage()
    exit(1)

# Check if the directories given as arguments are directories and exists.
if not (os.path.isdir(sources_dir) or os.path.isfile(sources_dir)) :
    print("ERROR : \"{0}\" is not a directory,or file or doesn't exists.".format(sources_dir))
    print_usage()
    exit(1)
if not os.path.isdir(scop_dir) :
    print("ERROR : \"{0}\" is not a directory or doesn't exists.".format(scop_dir))
    print_usage()
    exit(1)
if not os.path.isdir(aggr_scops_dir) :
    print("ERROR : \"{0}\" is not a directory or doesn't exists.".format(aggr_scops_dir))
    print_usage()
    exit(1)
if not os.path.isdir(res_dir) :
    print("ERROR : \"{0}\" is not a directory or doesn't exists.".format(res_dir))
    print_usage()
    exit(1)

type_list = sorted(next(os.walk(aggr_scops_dir))[1])
rate_list = sorted(next(os.walk("{0}/{1}/".format(aggr_scops_dir, type_list[0])))[1])
if os.path.isdir(sources_dir) :
    original_working_directory = os.getcwd()
    os.chdir(sources_dir)
    source_list = sorted(glob.glob("*.c"))
    if "polybench.c" in source_list : source_list.remove("polybench.c")
    os.chdir(original_working_directory)
else :
    tmp = os.path.split(sources_dir)
    sources_dir = tmp[0]
    source_list = [tmp[1]]

results={y:{z:None for z in type_list} for y in rate_list}
for source in source_list :
    print(source)
    print("pluto")
    res=benchmark(
            "{0}/{1}".format(sources_dir,source),
            "{0}/{1}.scop".format(scop_dir,source))
    results["pluto"]=res

    for rate in rate_list :
        print(rate)
        for _type in type_list :
            print(_type)
            res=benchmark(
                "{0}/{1}".format(sources_dir,source),
                "{0}/{1}/{2}/{3}.scop".format(aggr_scops_dir, _type, rate, source))
            results[rate][_type]=res
    gnuplot_data(results, source, type_list, rate_list)
