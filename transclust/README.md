# TransClust++ 

## Table of Content
* [Setup](#setup)
	* [Dependencies](#dependencies)
	* [Automated setup](#automated-setup)
	* [Manual setup](#manual-setup)
* [Usage](#usage)


## Setup
### Dependencies
This project depends on:
	- [CMake](https://cmake.org/) 
	- [Boost C++ Libraries](http://www.boost.org/)
	- [Open MPI](https://www.open-mpi.org/)
	- [Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html)
	- [Graphviz](http://www.graphviz.org/)

### Automated setup
For automated setup run `./SETUP`. 

This will initializes build directory's, build types, submodules, documentation 
and compile the project. If something fails during compilation, make sure that 
all dependencies are correctly installed.

### Manual setup

```Bash
	# Make sure all submodules are initialized
	git submodule update --init --recursive

	# Build documentation
	doxygen Doxyfile

	# Create build dirs
	mkdir -p build/release
	cd build/release

	# Initialize release build
	cmake -DCMAKE_BUILD_TYPE=Release ../..

	# compile
	make
```
## Usage
After setup has completed without error you can find the `tclust` binary in:
	
	build/release/bin/tclust

Basic usage on local machine:

	./build/release/bin/tclust -s <similarity file> -t <threshold> -o <output file>

Basic usage with MPI:

	mpirun -n <number of processes> ./build/release/bin/tclust -s <similarity file> -t <threshold> -o <output file>

For more:

	./build/release/bin/tclust --help

