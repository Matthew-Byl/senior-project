This is the code for [John Kloosterman's](http://jkloosterman.net) senior project at [Calvin College](http://cs.calvin.edu).

* cl_test: A C++ framework around OpenCL that removes some of OpenCL's tedium and makes it possible to call non-kernels directly.
* raytracer: An OpenCL raytracer with a GTK+ UI.
* kalah: An OpenCL implementation of the minimax AI algorithm for [Kalah](http://en.wikipedia.org/wiki/Kalah).
* societies: An OpenCL reimplementation of parts of the economics simulator [SugarPy](http://abs.calvin.edu/hg/sugarpy/).
* local_malloc: A Clang-based tool to rewrite OpenCL kernels to support a malloc()-like dynamic allocation of workgroup-local memory.
* reports: the LaTeX source and output for the final report about the project.

Building
--------
The top-level Makefile will build cl_test, kalah, and the raytracer.

There are references to the OpenCL library and include paths in most Makefiles. You will need to edit these if you are not using the AMD APP SDK on 64-bit Linux.

Dependencies
------------
* cl_test: OpenCL 1.1 or 1.2
* raytracer: GTK+ 2, libpng 1.2.49
* local_malloc: Source and a build of LLVM and Clang version 3.2
