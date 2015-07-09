#!/usr/bin/python3

import os, os.path, glob, re
from sys import argv
import time
from statistics import *
from subprocess import call

devnull = open(os.devnull, 'w')


nb_run=5
pluto_output_filename="/tmp/pluto_output"
pluto_options=["--parallel",
        #"--tile",
        ]

#Gnuplot command printed in the data file (to be able to directly
#plot the data
gnuplot_commands = "#Gnuplot datafile"

#Gnuplot comment printed before the data, explaining the format
gnuplot_format = """\
# Format : min_rate_for_aggregation speedup_pluto_TYPE.... speedup_pluto_original_scop
# with TYPE being 'reuse', 'parallelism', 'vectorization' etc..."""

def cleanup():
    files = [".unroll", ".vectorize", ".pragmas",  ".params", ".orcc", ".linearized",
            ".nonlinearized", "*.pluto.cloog", ".srcfilename", ".outfilename",
            ".distmem", "pi.cloog", "sigma.cloog", "*.sysloog", ".appendfilename"]
    for f in files :
        for g in glob.glob(f):
            os.remove(g)
    os.remove(pluto_output_filename+".pluto.cloog")
    os.remove(pluto_output_filename)

def benchmark(source_filepath, scop_filepath, output_filepath) :
    res = []
    pluto_cmd=[pluto_path,
            "--readscop",
            scop_filepath,
            "-o",
            pluto_output_filename]
    pluto_cmd=pluto_cmd+pluto_options
    inscop_cmd=[
            inscop_path,
            source_filepath,
            pluto_output_filename,
            output_filepath]

    for run in range(0,nb_run) :
        before=time.time()
        if call(pluto_cmd, stdout=devnull, stderr=devnull) is not 0 :
            print(pluto_cmd)
            raise Exception("Pluto call failed")
        after=time.time()
        res.append(after - before)
    
    if call(inscop_cmd, stdout=devnull) is not 0 :
        print(inscop_cmd)
        raise Exception("inscop call failed")

    cleanup()
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
            for type_ in type_list :
                substrate_res=results[type_][rate]
                substrate_res=1.0 + (pluto_res - substrate_res) / pluto_res
                output.write("\t"+('%.3f' % substrate_res))
            output.write("\n")


def print_usage() :
    print("Usage : {0} <pluto_path> <inscop_path> <sources_directory> \
<original_scops_directory> <optimized_scops_directory> <results_directory>".format(argv[0]))



# Check if there is enough argument
if len(argv) < 7 :
    print("ERROR : Not enough arguments.")
    print_usage()
    exit(1)

pluto_path=argv[1]
inscop_path=argv[2]
sources_dir=argv[3]
scop_dir=argv[4]
opti_scops_dir=argv[5]
res_dir=argv[6]

if not os.path.isfile(pluto_path) :
    print("ERROR : pluto program \"{0}\" doesn't exists.".format(substrate_bin))
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
if not os.path.isdir(opti_scops_dir) :
    print("ERROR : \"{0}\" is not a directory or doesn't exists.".format(opti_scops_dir))
    print_usage()
    exit(1)
if not os.path.isdir(opti_scops_dir) :
    print("ERROR : \"{0}\" is not a directory or doesn't exists.".format(res_dir))
    print_usage()
    exit(1)

if not os.path.isdir(opti_scops_dir+"/pluto") :
    os.mkdir(opti_scops_dir+"/pluto")

type_list = sorted(next(os.walk(opti_scops_dir))[1])
type_list.remove("pluto")
rate_list = sorted(next(os.walk("{0}/{1}/".format(opti_scops_dir, type_list[0])))[1])
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

results={y:{z:None for z in rate_list} for y in type_list}
for source in source_list :
    print(source)
    print("\tpluto")
    res=benchmark(
            "{0}/{1}".format(sources_dir,source),
            "{0}/{1}.scop".format(scop_dir,source),
            "{0}/pluto/{1}".format(opti_scops_dir,source))
    results["pluto"]=res

    for type_ in type_list :
        print("\t\t"+type_)
        for rate in rate_list :
            print("\t\t\t"+rate)
            res=benchmark(
                "{0}/{1}".format(sources_dir,source),
                "{0}/{1}/{2}/{3}.scop".format(opti_scops_dir, type_, rate, source),
                "{0}/{1}/{2}/{3}".format(opti_scops_dir, type_, rate, source))
            results[type_][rate]=res
    gnuplot_data(results, source, type_list, rate_list)
