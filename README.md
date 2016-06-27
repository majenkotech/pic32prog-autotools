This is a fork of Serge's PIC32Prog with an improved cross-platform build system
using GNU AutoTools.

Compiling:
---------

    $ autoreconf -fi
    $ ./configure
    $ make

Supported Platforms
-------------------

Compiling is known to work on:

* Linux (Debian Jessie, etc)
* FreeBSD 10.3
* OpenBSD 5.9
* OS X Snow Leopard

Cross-compiling on Linux is known to work for:

* Windows (i686-w64-mingw)
* OpenWRT MIPS - Arduino Yun, Dragino, etc, without USB drivers (mips-linux-uclibc)
* ARM Linux - Raspberry Pi, etc (arm-linux-gnueabihf)

Cross-compiling for Windows:
----------------------------

    $ autoreconf -fi
    (you can skip that if already done before)
    $ ./configure --host=i686-w64-mingw32
    $ make

Enabling and disabling protocols:
--------------------------------

All protocols are enabled by default. Disable each one with

    $ ./configure --disable-<protocol>

For the full list run:

    $ ./configure --help

OS X Fat Binaries:
-----------------

By default on OS X fat binaries (32-bit + 64-bit) will be built. This depends
on libusb-1.0 being built in *universal* mode on your system (using Homebrew).

To disable one of the architectures if you don't have a universal libusb you can:

    $ ./configure --disable-32bit

or:

    $ ./configure --disable-64bit

