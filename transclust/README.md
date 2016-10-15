# Setup project
Initializes build directory, build types and submodules
```Bash
	./SETUP.sh
```

#How to build
This project uses [CMake](https://cmake.org/) and [Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html)

Run `SETUP.sh` first
```Bash
	cd build/release
	make
```

#Rebuild doc
From the root run
```Bash
   doxygen Doxyfile
   
```
