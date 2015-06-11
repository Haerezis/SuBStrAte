#!/usr/bin/python3

###############
## Script to use to transform speedup per rate results to speedup per nb_stmt
###############


import os, os.path, glob, re
from sys import argv
import time
from statistics import *
from subprocess import call

#Gnuplot command printed in the data file (to be able to directly
#plot the data
gnuplot_commands = "#Gnuplot datafile"

#Gnuplot comment printed before the data, explaining the format
gnuplot_format = """\
# Format : min_rate_for_aggregation speedup_TYPE.... speedup_scop
# with TYPE being 'reuse', 'parallelism', 'vectorization' etc..."""


def nb_stmt_file_to_dict(filename) :
    res = {}
    type_ = []
    with open(filename,"r") as input_file :
        for line in input_file :
            if line[0] == '#' :
                if line.startswith("#rate") :
                    line = re.split(" |\t|\n",line[1:])
                    type_ = list(filter(len,line))[1:]
            elif line[0] == ' ' :
                line = re.split(" |\t|\n",line[1:])
                rate = line[0]
                res[rate] = {}
                for ty,val in zip(type_,list(filter(len,line[2:]))) :
                    res[rate][ty] = int(val)
    return res

def runtime_file_to_dict(filename) :
    mean_reached=False
    res = {}
    type_ = []
    with open(filename,"r") as input_file :
        for line in input_file :
            if line[0] == '#' :
                if line.startswith("#rate") :
                    line = re.split(" |\t|\n",line[1:])
                    type_ = list(filter(len,line))[1:]
            elif line[0] == ' ' :
                mean_reached=True
                line = re.split(" |\t|\n",line[1:])
                rate = line[0]
                res[rate] = {}
                for ty,val in zip(type_,list(filter(len,line[1:]))) :
                    res[rate][ty] = float(val)
            elif mean_reached : break
    return res

def gnuplot_data(results, source_filename, type_list) :
    res_filepath = "{0}/{1}.results".format(output_dir, source_filename)
    with open(res_filepath, "w") as output :
        output.write(gnuplot_commands+"\n\n")

        output.write("#MEAN\n")
        output.write(gnuplot_format+'\n')
        
        output.write("#")
        output.write("nb_stmt")
        #output.write("\tpluto")
        for k in sorted(type_list) : output.write(" \t"+k)
        output.write('\n')
        for nb_stmt in sorted(results) :
            output.write(" "+str(nb_stmt))
            #output.write(" \t%.3f" % mean((pluto_res)))
            for type_ in type_list :
                output.write(", ")
                if results[nb_stmt].get(type_,None) is not None :
                    res_m=mean(results[nb_stmt][type_])
                    if len(results[nb_stmt][type_]) == 1 :
                        res_sd=0.0
                    else :
                        res_sd=float(stdev(results[nb_stmt][type_]))
                    output.write('%.2f,%.2f' % (res_m,res_sd))
                else :
                    output.write(" , ")
            output.write("\n")
        
        #output.write("\n\n")
        #output.write("#MEDIAN\n")
        #output.write(gnuplot_format+'\n')
        
        #output.write("#")
        #output.write("rate")
        ##output.write("\tpluto")
        #for k in sorted(type_list) : output.write(" \t"+k)
        #output.write('\n')
        #for rate in rate_list :
            #output.write(" {0}".format(rate,median(pluto_res)))
            ##output.write(" \t%.3f" % (median(pluto_res)))
            #for type_ in type_list :
                #substrate_res=results[type_][rate]
                #substrate_res=1.0 + ((median(pluto_res) - median(substrate_res))) / median(pluto_res)
                #output.write("\t"+('%.2f' % substrate_res))
            #output.write("\n")


def print_usage() :
    print("Usage : {0} <original_source_directory | original_source_filepath>\
 <nb_stmt_results_directory> <runtime_results_directory> <output_directory>".format(argv[0]))



# Check if there is enough argument
if len(argv) < 5 :
    print("ERROR : Not enough arguments.")
    print_usage()
    exit(1)

original_sources_dir=argv[1]
nb_stmt_res_dir=argv[2]
runtime_res_dir=argv[3]
output_dir=argv[4]

if not os.path.isdir(original_sources_dir) and not os.path.isfile(original_sources_dir): 
    print("ERROR : original sources directory or file \"{0}\" doesn't exists.".format(original_sources_dir))
    print_usage()
    exit(1)
if not os.path.isdir(nb_stmt_res_dir) : 
    print("ERROR : nb_stmt results directory \"{0}\" doesn't exists.".format(nb_stmt_results_directory))
    print_usage()
    exit(1)
if not os.path.isdir(runtime_res_dir) :
    print("ERROR : runtime results directory \"{0}\" doesn't exists.".format(runtime_results_directory))
    print_usage()
    exit(1)
if not os.path.isdir(output_dir) :
    print("ERROR : output directory \"{0}\" doesn't exists.".format(output_directory))
    print_usage()
    exit(1)



if os.path.isdir(original_sources_dir) :
    original_working_directory = os.getcwd()
    os.chdir(original_sources_dir)
    source_list = sorted(glob.glob("*.c"))
    if "polybench.c" in source_list : source_list.remove("polybench.c")
    os.chdir(original_working_directory)
else :
    tmp = os.path.split(original_sources_dir)
    original_sources_dir = tmp[0]
    source_list = [tmp[1]]

for source in source_list :
    nb_stmt_res = nb_stmt_file_to_dict("{0}/{1}.scop.results".format(nb_stmt_res_dir,source))
    runtime_res = runtime_file_to_dict("{0}/{1}.results".format(runtime_res_dir,source))

    rates = sorted(nb_stmt_res)
    types = sorted(runtime_res[rates[0]])

    res = {}
    for rate in rates :
        for type_ in types :
            nb_stmt = nb_stmt_res[rate][type_]
            val = runtime_res[rate][type_]
            if res.get(nb_stmt, None) is None :
                res[nb_stmt] = {}
            if res[nb_stmt].get(type_, None) is None :
                res[nb_stmt][type_] = []
            res[nb_stmt][type_].append(val)
    #print(res)
    gnuplot_data(res,source,types)
