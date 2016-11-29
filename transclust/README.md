# Setup project
Initializes build directory, build types, submodules and compiles the project
```Bash
	./SETUP.sh
```

#How to build withohut SETUP.sh
This project uses 
[CMake](https://cmake.org/), 
[Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html),
[Boost C++ Libraries](http://www.boost.org/),
[Open MPI](https://www.open-mpi.org/)

Run `SETUP.sh` first
```Bash
	mkdir build/release
	cd build/release
	cmake -DCMAKE_BUILD_TYPE=Release ../..
	make
```

#Rebuild doc
From the root run
```Bash
   doxygen Doxyfile
   
```


# Usage
After setup has completed without errer you can find the tclust binary in:
	
	build/release/bin/tclust

Basic usage on local machine:

	./build/release/bin/tclust -s <similarity file> -t <threshold> 

Basic usage with MPI:

	mpirun -n <number of processes> ./build/release/bin/tclust -s <similarity file> -t <threshold> 

For more:

	./build/release/bin/tclust --help

