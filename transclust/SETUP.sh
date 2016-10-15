# clean
rm -r -f build

# make sure all submodules are initialized
git submodule update --init --recursive

# setup build dir
mkdir build
mkdir build/debug
mkdir build/release

# init cmake build versions
cd build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
cd ../release
cmake -DCMAKE_BUILD_TYPE=Release ../..
