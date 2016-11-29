cat << EOF

+-------------------------------------------------------------------------------+
|  TransClust++ Setup Script                                                    |
+-------------------------------------------------------------------------------+

EOF
# clean
#rm -r -f build

# make sure all submodules are initialized
git submodule update --init --recursive

# setup build dir
mkdir build
mkdir build/release

doxygen Doxyfile

# init cmake build versions
#mkdir build/debug
#cd build/debug
#cmake -DCMAKE_BUILD_TYPE=Debug ../..
cd build/release
cmake -DCMAKE_BUILD_TYPE=Release ../..
# make release version
make

cat << EOF

+-------------------------------------------------------------------------------+
|  Done                                                                         |
+-------------------------------------------------------------------------------+

   If the installation complete with errors remember to install: 
      Cmake                >= 3.5.1
      Boost C++ Libraries  >= 1.58
      Open MPI             >= 10.1.2
      doxygen              >= 1.8.11

+-------------------------------------------------------------------------------+
|  Usage                                                                        |
+-------------------------------------------------------------------------------+
   After setup has completed without errer you can find the tclust binary in:

      build/release/bin/tclust

   Basic usage on local machine:

      ./build/release/bin/tclust -s <similarity file> -t <threshold> 

   Basic usage with MPI:

      mpirun -n <number of processes> ./build/release/bin/tclust -s <similarity file> -t <threshold> 

   For more:

      ./build/release/bin/tclust --help

EOF
