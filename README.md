# sofa-designer

The program is the computer-assited part of a prospected proof on the moving sofa problem in progress.
It is developed by [Jineon Baek](https://jcpaik.github.io/).

An essential idea of Gerver is to see the movement in perspective of the moving sofa, 
so that the sofa is fixed and the hallway rotates and translates around it.
Then the sofa is essentially the intersection of the hallways being rotated counterclockwise from 0 to 90 degrees and translated.
The problem can be approximated by intersecting only a finite number of hallways in finite angles,
and Gerver proved that the optimum to this approximation converges to the optimal sofa.

`SofaDesigner` does the followings.

1. It computes every possibility of the intersection of hallways rotated by a given finite list of angles.
2. It proves lower and upper bound on any linear functional on the locations of hallways by examining each possibility.

## Usage

**The software is still in development and the details are subject to change.**

The angles are represented as a single JSON list.
```json
[
    {"cos": "12/13", "sin": "5/13", "branch_order": 3},
    {"cos": "4/5", "sin": "3/5", "branch_order": 1},
    {"cos": "3/5"  , "sin": "4/5"  , "branch_order": 0},
    {"cos": "5/13"  , "sin": "12/13"  , "branch_order": 2}
]
```
The angles should be strictly in between 0 and 90 degrees and have rational `cos` and `sin` values.
The field `branch_order` determines the order in which the hallway is included in the main branch-and-bound algorithm.

Once the program is built, do 
```bash
./sbranch --angles angles.json --out angles.crl
```
to store every possibilities of the intersection of hallways rotated by angles in `angles.json` in a file `angles.crl`.
The `--show-max-area` flag also computes the maximum possible area of such intersection, 
giving an upper bound of the area of any sofa rotating by 90 degrees.

Then, use the `angles.crl` file to prove lower/upper bound of any linear functional as the following.
```bash
./sprove angles.crl "dot(A(0)-A(5),u(0))" --lb 0 --ub 1
```

See `runs/` directory for more examples.

## Structure

`lib` contains main, common libraries developed for the project. 
Currently the only library is `sofa`.

`bin` contains source code for the final binaries `sbranch` and `sprove`.

`test` contains test codes based on `catch2` library.
A single executable `run_tests` runs all tests (except hidden tests).

`extern` contains external libraries the project depends on.

## How to build

Install `cmake` and libraries `gmp` and `cgal`. This is

    brew install cmake gmp cgal catch2

on MacOS. Check respective repository for other OS.
I found successful compilation under the following versions.

- CGAL 5.5.2
- GMP 6.2.1\_1
- CMake 3.26.3
- Catch2 3.4.0

To build, make and `cd` to the `build` directory and run `cmake ..; make`.

    mkdir build
    cd build
    cmake ..
    make

The binaries will appear in `build/bin`.

## Acknowledgements

[@jwvg0425](https://github.com/jwvg0425) has helped in shaping the program architecture
and making the code to follow better design patterns.
