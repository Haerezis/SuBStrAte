#Substrate dev guide

The purpose of this guide is to help newcomer developers on this project.

##How substrate work

As an input, substrate take an openscop file, and produce a new and optimized openscop file from it.
You can specify some options to substrate, like profile type weight and minimal rate for aggregation.

The input and output FILE* and options are stored in a global structure, declared externally
in "options.h". To know more about the different options and the structure, you can look at
the files "options.h" and "options.c".

###Analyze
For each statement of each scop contained in the input openscop file, substrate will compute
and build a profile.

This profile is composed of several type of profile :
 * Data Reuse
 * Parallelism
 * Vectorization
 * Tiling Hyperplane
and more to come.

Substrate will give the current analyzed statement to the constructor function of every
profile type : this function will compute and build the profile, according to some algorithm.

All the sub-profile are then put together into a structure ```substrate_statement_profile```
and attached to the statement in the _usr_ field.

All of this is done in the function ```substrate_analyze```.

###Optimization

Once the scops contained int he input file were analyzed and their profiles build, substrate
try to aggregate statements of the current optimized scop according to an aggregation
strategy/algorithm.

For now, there is 2 aggregation strategies :
 - Successive Statement Aggregation (SSA) : try to aggregate successive statements from
 the first to last of the scop : to be aggregated, 2 statements need to have the same
 domain, same scattering and an affinity/similarity rate greater than the minimal
 aggregation rate (substrate options).
 - Greedy Graph Aggregation (GGA) : try to aggregate statements even if they are not
 next to each other. This strategy use a greedy algorithm on the affinity/similarity rate
 to chose which statements are aggregated. Additionally, to be aggregated, the 2 chosen
 need to be able to be next to each other, so we check that it is legal to move one of the
 2 statements. The 2 statements also need to have the same domain and scattering (except
 beta vector).

All of this is done in "optimization.c".

##Add a new profile type X
 1. Write the profile type structure files : X_struct.[c|h]. These files only
 contained the allocation/free/dump/clone/fusion functions.
 2. Write the profile type files : X.[c|h]. These files contained the constructor and
 rating function for this profile type.
 3. Add this profile type to the ```substrate_statement_profile``` structure.
 4. Add the call to the malloc/free/dump/clone/fusion functions in the corresponding
 functions of the ```substrate_statement_profile``` structure in "statement_profile.c".
 5. Add the call to the rating function  in the ```substrate_similarity_rate``` function
 in "optimization.c".


##Add new aggregation strategies
 1. Write the new strategy, with only the profiled scop as argument, returning an
 newly allocated (or cloned) scop.
 2. Add a new symbol in the ```substrate_aggregation_strategy``` enum in "enum.h".
 3. Add a case for this new enum symbol in switch in the ```substrate_optimize``` function in
 "optimize.h", to call the new strategy function.
 4. Add an option to substrate to use this strategy in "option.c" 
