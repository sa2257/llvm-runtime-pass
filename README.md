# llvm-runtime-pass

An LLVM template pass to use runtime function libraries.
It's for LLVM 9.

Build:

    $ cd llvm-runtime-pass
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ cd ..

Run:

    $ clang -S -emit-llvm -Xclang -disable-O0-optnone foo.c
    $ opt -load build/skeleton/libSkeletonPass.* -skeleton -S foo.ll
    
The `-Xclang -disable-O0-optnone` flag ensures that Clang will allow later optimizations even when initially compiling without any. 
