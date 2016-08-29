# IMPORTANT!
Run 
```Bash
git submodule update --recursive
```
To pull all submodules

#How to build
This project uses [CMake](https://cmake.org/) and [Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html)

From the root run:
```Bash
   mkdir build
   cd build
   cmake ..
   make
```

#Run example
From the build folder run:
```Bash
   ./bin/transclust ../data/karateclub/Zachary_karate_club_similarities.sim
```
For help run:
```Bash
   ./bin/transclust --help
```

#Rebuild doc
From the root run
```Bash
   doxygen Doxyfile
   
```
