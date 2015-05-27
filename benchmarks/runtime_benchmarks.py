#!/usr/bin/python3

import os, os.path, glob, re
from sys import argv
import time
from statistics import *
from subprocess import call

devnull = open(os.devnull, 'w')


nb_run=5
bin_output_filename="/tmp/a.out"
gcc_flags=["-O3","-lm","-fopenmp",]# "-DEXTRALARGE_DATASET"]

#Gnuplot command printed in the data file (to be able to directly
#plot the data
gnuplot_commands = "#Gnuplot datafile"

#Gnuplot comment printed before the data, explaining the format
gnuplot_format = """\
# Format : min_rate_for_aggregation speedup_TYPE.... speedup_scop
# with TYPE being 'reuse', 'parallelism', 'vectorization' etc..."""


def benchmark(source_filepath) :
    res = []
    gcc_cmd = ["gcc",
            "-o",
            bin_output_filename,
            source_filepath,
            original_sources_dir+"/polybench.c",
            "-I",
            original_sources_dir]
    gcc_cmd=gcc_cmd+gcc_flags
    if call(gcc_cmd, stdout=devnull) is not 0 :
        print(gcc_cmd)
        raise Exception("gcc call failed")

    for run in range(0,nb_run) :
        before=time.time()
        call(["chrt", "-f", "99", bin_output_filename], stdout=devnull)
        after=time.time()
        res.append(after - before)

    return res

def gnuplot_data(results, source_filename, type_list, rate_list) :
    pluto_res=results["pluto"]
    res_filepath = "{0}/{1}.results".format(res_dir, source_filename)
    with open(res_filepath, "w") as output :
        output.write(gnuplot_commands+"\n\n")


        output.write("#MEAN\n")
        output.write(gnuplot_format+'\n')
        
        output.write("#")
        output.write("rate")
        #output.write("\tpluto")
        for k in sorted(type_list) : output.write(" \t"+k)
        output.write('\n')
        for rate in rate_list :
            output.write(" {0}".format(rate,mean(pluto_res)))
            #output.write(" \t%.3f" % mean((pluto_res)))
            for type_ in type_list :
                substrate_res=results[type_][rate]
                substrate_res=1.0 + (mean(pluto_res) - mean(substrate_res)) / mean(pluto_res)
                output.write("\t"+('%.2f' % substrate_res))
            output.write("\n")
        
        output.write("\n\n")
        output.write("#MEDIAN\n")
        output.write(gnuplot_format+'\n')
        
        output.write("#")
        output.write("rate")
        #output.write("\tpluto")
        for k in sorted(type_list) : output.write(" \t"+k)
        output.write('\n')
        for rate in rate_list :
            output.write(" {0}".format(rate,median(pluto_res)))
            #output.write(" \t%.3f" % (median(pluto_res)))
            for type_ in type_list :
                substrate_res=results[type_][rate]
                substrate_res=1.0 + ((median(pluto_res) - median(substrate_res))) / median(pluto_res)
                output.write("\t"+('%.2f' % substrate_res))
            output.write("\n")


def print_usage() :
    print("Usage : {0} <original_source_directory | original_source_filepath>\
 <optimized_sources_directory> <results_directory>".format(argv[0]))



# Check if there is enough argument
if len(argv) < 4 :
    print("ERROR : Not enough arguments.")
    print_usage()
    exit(1)

original_sources_dir=argv[1]
optimized_sources_dir=argv[2]
res_dir=argv[3]

if not os.path.isdir(original_sources_dir) : 
    print("ERROR : original sources directory \"{0}\" doesn't exists.".format(original_sources_dir))
    print_usage()
    exit(1)
if not os.path.isdir(optimized_sources_dir) : 
    print("ERROR : sources directory \"{0}\" doesn't exists.".format(optimized_sources_dir))
    print_usage()
    exit(1)
if not os.path.isdir(optimized_sources_dir+"/pluto") :
    print("ERROR : sources directory \"{0}\" doesn't have a 'pluto' subdirectory.".format(optimized_sources_dir))
    print_usage()
    exit(1)
if not os.path.isdir(res_dir) :
    print("ERROR : results directory \"{0}\" doesn't exists.".format(res_dir))
    print_usage()
    exit(1)

if os.geteuid() != 0 :
   print("ERROR : You need root privileges to run this script")
   exit(1)


type_list = sorted(next(os.walk(optimized_sources_dir))[1])
type_list.remove("pluto")
rate_list = sorted(next(os.walk("{0}/{1}/".format(optimized_sources_dir, type_list[0])))[1])
if os.path.isdir(original_sources_dir) :
    original_working_directory = os.getcwd()
    os.chdir(original_sources_dir)
    source_list = sorted(glob.glob("*.c"))
    if "polybench.c" in source_list : source_list.remove("polybench.c")
    os.chdir(original_working_directory)
else :
    tmp = os.path.split(optimized_sources_dir)
    optimized_sources_dir = tmp[0]
    source_list = [tmp[1]]

results={y:{z:None for z in rate_list} for y in type_list}
for source in source_list :
    print(source)
    print("\tpluto")
    res=benchmark("{0}/pluto/{1}".format(optimized_sources_dir,source))
    results["pluto"]=res

    for type_ in type_list :
        print("\t\t"+ type_)
        for rate in rate_list :
            print("\t\t\t"+rate)
            res=benchmark(
                "{0}/{1}/{2}/{3}".format(optimized_sources_dir, type_, rate, source))
            results[type_][rate]=res
    
    gnuplot_data(results, source, type_list, rate_list)
