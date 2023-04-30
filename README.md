# sofa-designer

## What it is

The program is the computer-assited part of a proof of the optimality of Gerver's sofa in progress.
It is developed by [Jineon Baek](https://jcpaik.github.io/). 

## Structure

`libs` contains main, common libraries developed for the project. 
Currently the only library is `sofa`.

`apps` contains applications using libraries in `libs`. 
Each step of the computer-assisted proof is implemented here.
Each `*.cc` file corresponds to a single executable of the same name.

`tests` contains test codes based on `catch2` library.
A single executable `run_tests` runs all tests (except hidden tests).

`extern` contains external libraries the project depends on.

`docs` contains documentation files.

`sketches` contain computations 'out of the project'. 
The computations may be used as inputs to this project,
or used to cross-check the results of this project.

## How to build

Install `cmake` and libraries `gmp` and `cgal`. This is

    apt-get install cmake libgmp-dev libcgal-dev

on Ubuntu. Check respective repository for other OS.
I found successful compilation under the following versions.

- CGAL 5.5.2
- GMP 6.2.1\_1
- CMake 3.26.3

To build, make and `cd` to the `build` directory and run `cmake ..; make`.

    mkdir build
    cd build
    cmake ..
    make

The binaries will appear in `build/bin`.

Although you can call `cmake .; make` directly from the project root directory,
it is not recommended since it will mix source codes with compiled results. 

## Binaries

- `build/bin/run_tests` runs all tests for the project.

## Acknowledgements

[@jwvg0425](https://github.com/jwvg0425) has helped in shaping the program architecture
and making the code to follow better design patterns.
