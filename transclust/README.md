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
   ./bin/transclust ../data/brown/sfld_brown_et_al_amidohydrolases_protein_similarities_for_beh.txt
```

#Rebuild doc
From the root run
```Bash
   doxygen Doxyfile
   
```
