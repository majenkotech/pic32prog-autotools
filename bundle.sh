#!/bin/bash

VER=`bin/pic32prog-debian-64 2>/dev/null | grep Version | awk '{print $7}'`

mkdir -p release

function bundle {
    SRC=$1
    OS=$2
    mkdir -p tmp/pic32prog
    if [ "$OS" = "windows" ]; then
        cp $SRC tmp/pic32prog/pic32prog.exe
    else
        cp $SRC tmp/pic32prog/pic32prog
    fi

    tar -C tmp -zcf release/pic32prog-${OS}-${VER}.tar.gz pic32prog
    rm -rf tmp
    (
        cd release
        sha256sum pic32prog-armlinux-2.1.24.tar.gz | awk '{print "\nAlgorithm       Hash                                                                   Path\n---------       ----                                                                   ----\nSHA256          " toupper($1) "       " $2 "\n\n"}' > pic32prog-${OS}-${VER}.asc
    )
}

bundle bin/pic32prog-debian-64 linux64
bundle bin/pic32prog-debian-32 linux32
bundle bin/pic32prog-debian-armhf armlinux
bundle bin/pic32prog-win32.exe windows
bundle bin/pic32prog-osx-fat macosx
