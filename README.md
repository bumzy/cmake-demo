# cmake-demo
C++ project demo build with CMake using third party gRPC, gtest and JSON

## Environment
### Ubuntu
 * `sudo apt install cmake build-essential`

## Build
 * cmake version >= 3.5
 * `git clone --recursive ...`
 or `git pull && git submodule update --recursive --init`
 * `mkdir build && cd build`
 * `cmake ..`
 * `make`

## Add Third Party
 * `mkdir -p third_party && cd third_party`
 * `git submodule add <repository>`
 * `cd repository_dir`
 * `git tag` select release vX.X.X
 * `git checkout tags/vX.X.X`
 * `cd ../../ && git add third_party/repository_dir && git commit -m '...' && git push`

## CMake Example
 * [ttroy50/cmake-examples](https://github.com/ttroy50/cmake-examples)