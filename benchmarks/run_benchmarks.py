#!/usr/bin/python

import os, os.path, glob, re
from sys import argv
from subprocess import call

###GLOBAL VARIABLES
#We will try to benchmark for rate from rate_begin to rate_end
#with a increment of rate_int
rate_begin=0.0
rate_end=1.0
rate_inc=0.1

#The path of the different directory/binary needed (should be given
#as the program argument).
substrate_bin="."
scops_dir="."
aggr_scops_dir="."

#Gnuplot command printed in the data file (to be able to directly
#plot the data
gnuplot_commands = "#Gnuplot datafile"

#Gnuplot comment printed before the data, explaining the format
gnuplot_format = """\
# Format : min_rate_for_aggregation nb_aggregated_statement_TYPE.... nb_statement_original_scop
# with TYPE being 'reuse', 'parallelism', 'vectorization' etc..."""


def print_usage() :
    print "Usage : {0} <substrate_path> <scops_directory> <aggregated_scops_directory>"\
            .format(argv[0])


profile_options = {
        "reuse" : "--reuse-weight",
        "parallelism" : "--paral-weight",
        "vectorization" : "--vector-weight",
        "tiling_hyperplane" : "--tiling-weight" }


def substrate(profile_type_list , rate_list, scop_filename_list) :
    for profile_type in profile_type_list :
        directory = "{0}/{1}".format(aggr_scops_dir,profile_type)
        if not os.path.exists(directory) :
            os.makedirs(directory)

        for rate in rate_list :
            directory_rate = "{0}/{1}".format(directory,rate)
            if not os.path.exists(directory_rate) :
                os.makedirs(directory_rate)

            for scop_filename in scop_filename_list :
                command = [substrate_bin,
                        "-o",
                        "{0}/{1}".format(directory_rate,scop_filename),
                        "--min-rate",
                        str(rate),
                        profile_options[profile_type],
                        "1.0",
                        "{0}/{1}".format(scops_dir,scop_filename)]
                #print command
                call( command )

def nb_stmt_in_scop_file(scop_filepath) :
    with open(scop_filepath, "r") as scop :
        for line in scop :
            if re.search("# Number of statements",line) :
                line = scop.next()
                return int(line)
    return 0

def gnuplot_data(profile_type_list, rate_list, scop_filename_list) :

    for scop_filename in scop_filename_list :
        results = {key: [] for key in ["rate"]+profile_type_list+["nb_stmt_original"]}
        for rate in rate_list :
            results["rate"].append(rate)
            nb_stmt_original = nb_stmt_in_scop_file("{0}/{1}".format(scops_dir,scop_filename))
            results["nb_stmt_original"].append(nb_stmt_original)
            for profile_type in profile_type_list :
                directory = "{0}/{1}/{2}".format(aggr_scops_dir,profile_type,rate)
                nb_stmt = nb_stmt_in_scop_file("{0}/{1}".format(directory,scop_filename))
                results[profile_type].append(nb_stmt_original - nb_stmt)
        #write result in file
        res_filepath = "{0}/{1}.results".format(aggr_scops_dir, scop_filename)
        with open (res_filepath, "w") as res_file :
            res_file.write(gnuplot_commands+"\n\n")
            res_file.write(gnuplot_format+'\n')
            
            rate_list = results["rate"]
            del results["rate"]
            nb_stmt_original_list = results["nb_stmt_original"]
            del results["nb_stmt_original"]

            res_file.write("#")
            res_file.write("rate")
            res_file.write(" \tnb_stmt_original")
            for k in sorted(results) : res_file.write(" \t"+k)
            res_file.write('\n')
            for idx,rate in enumerate(rate_list) :
                res_file.write(" "+str(rate))
                res_file.write(" \t"+str(nb_stmt_original_list[idx]))
                for key in sorted(results) :
                    res_file.write(" \t" + str(results[key][idx]))
                res_file.write('\n')

                



# Check if there is enough argument
if len(argv) < 4 :
    print "ERROR : Not enough arguments."
    print_usage()
    exit(1)

substrate_bin=argv[1]
scops_dir=argv[2]
aggr_scops_dir=argv[3]

if not os.path.isfile(argv[1]) :
    print "ERROR : substrate program \"{0}\" doesn't exists.".format(substrate_bin)
    print_usage()
    exit(1)

# Check if the directories given as arguments are directories and exists.
if not os.path.isdir(scops_dir) :
    print "ERROR : \"{0}\" is not a directory or doesn't exists.".format(scops_dir)
    print_usage()
    exit(1)
if not os.path.isdir(aggr_scops_dir) :
    print "ERROR : \"{0}\" is not a directory or doesn't exists.".format(aggr_scops_dir)
    print_usage()
    exit(1)


# Creating aggregated scops
original_working_directory = os.getcwd()
os.chdir(scops_dir)
scops_list = glob.glob("*.scop")
os.chdir(original_working_directory)

# Creating the list of rate that will be computed
rate = rate_begin
rate_list = []
while rate <= rate_end :
    rate_str = ('%.5f' % (rate)).rstrip('0')
    if rate_str.endswith('.') : rate_str = rate_str + '0'
    rate_list.append(rate_str)
    rate = rate+rate_inc

# List of the profile type we're asking to benchmark
profile_type_list = profile_options.keys()
substrate(profile_type_list, rate_list, scops_list)
gnuplot_data(profile_type_list, rate_list, scops_list)
