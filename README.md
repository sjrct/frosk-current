Frosk
=====

Frosk is an operating system for x86-64/AMD64. It was also designed to eventually
support 32-bit x86 but that is not fully built out nor tested (after all, 64-bit
mode isn't even fully tested or working).

Building
--------

You must have `gcc`, `make`, `nasm` and `ar` (provided usually by `binutils`)
in order to build a Frosk disk image. Then just init the submodules and run `make` and it everything
should go hunky dory:

```sh
git submodule init
git submodule update
make
```

If `make` is giving you trouble make sure
you are using GNU make (`gmake`). You probably can't use `clang` as of now
because it doesn't have a builtin for the `floor`/`ceil` functions which I rely
on to implement those in the C standard library (I was lazy), but you can give
it a try by setting the `CC` and `LD` variables in `config.mk`.

If you are trying to build Frosk on a system that isn't x86-64/AMD64, you will
need to install or build a cross compiler, a version of `gcc` that will target
x86-64.  With `brew`, you can install `x86_64-elf-gcc` and then set the
following values in `config.mk`:

```Makefile
CC := x86_64-elf-gcc
LD := x86_64-elf-gcc
AR := x86_64-elf-ar
HOST_CC := gcc
HOST_LD := gcc
```

If you are installing a different cross compiler or building your own update `config.mk` following the above template. Note that you will also need a cross-platform version of `ar`.

The built disk image is a raw disk image, meaning it can be directly written to
a disk or partition with `dd`. You can run it easily in `qemu` with the
`./test` script.

I don't expect it will support many types of hardware very well, this isn't the
most tested operating system lol. If you try to run it on hardware, let me know
what you get out of it (good or bad!).

Philosophy
----------

[This is a bit outdated&mdash; perhaps all philosophies are, but especially one
invented as a joke by my younger and ultimately immature self, but I have
preserved it for posterity's sake. If there is any philosophy true of Frosk, it
is just that I could and wanted to build it so I did.]

The frosk philosophy aims for tranquility and mediation, to be one with the
source and find peace in bytes. Yet frosk, and its philosophy, are human
constructions, and as a result are inherently flawed, yet acceptance and
minimization of these flaws are what admirable goals we adhere too.

Whilst we do not deny the usefulness of optimization, it is not a high priority
for the frosk project, nor is pessimization. In fact, with regard to the
typical question of "Is the glass half full, or half empty?", we answer neither
with the positive nor with the negative, for the glass is neither half full nor
half empty; indeed, if you want a glass, you should implement it first.
