SuBStrAte, the Similar Behaving Statements Aggregator.

Substrate is a scop (Polyhedral Model / OpenScop) analyzer and optimizer.

It analyzes the the scop by creating a profile for each of its statements.
Each statement's profile is composed of subprofile : Data Reuse Profile, Parallelism Profile, Vectorization Profile, Tiling Hyperplane Profile...

It then optimize the scop by looking at the profile of each statement, and try to aggregate the ones that share some affinity/similarity according to some aggregation strategy.

Substrate depends on the following libraries:
- [OpenScop](https://github.com/periscop/openscop)
- [Candl](https://github.com/periscop/candl)
- [Clay](https://github.com/periscop/clay)
- [Pluto](http://repo.or.cz/w/pluto.git)

<!--Substrate can either use the the system wide installed version of these libraries, or local versions.-->
<!--For each library, if their directory (osl, candl, clay and pluto) is filled with the library project, then cmake will build it and use this local version. If it's not the case, it will use the system wide installed version.-->

<!--Each dependencies was added to this git as git submodule, so if you want to use the local version of a library, all you have to do is go in the library directory, and type :-->
<!--```-->
<!--git submodule update --init-->
<!--./get_submodules.sh-->
<!--```-->

To compile substrate :
```
cmake .
make
```
