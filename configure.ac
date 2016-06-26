#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.10])
AC_INIT([pic32prog], [2.1.0], [serge@vak.ru])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects no-dependencies])
#LT_INIT
AC_CONFIG_MACRO_DIR([m4])



# Checks for programs.
AC_PROG_CC
PKG_PROG_PKG_CONFIG
AC_CANONICAL_HOST

WANT_HID=0
WANT_USB=0
WANT_SER=0
ADAPTER_SRC=""
ADAPTER_OBJ=""
HID_SRC=""
HID_OBJ=""
SER_SRC=""
SER_OBJ=""

# First round decides if we want USB or not.
AC_ARG_ENABLE([an1388], AS_HELP_STRING([--disable-an1388], [Disable AN1388 HID adapter]))
AS_IF([test "x$enable_an1388" != "xno"], [
    WANT_HID=1
    WANT_USB=1
])

AC_ARG_ENABLE([hidboot], AS_HELP_STRING([--disable-hidboot], [Disable HidBoot adapter]))
AS_IF([test "x$enable_hidboot" != "xno"], [
    WANT_USB=1
    WANT_HID=1
])

AC_ARG_ENABLE([mpsse], AS_HELP_STRING([--disable-mpsse], [Disable MPSSE adapter]))
AS_IF([test "x$enable_mpsse" != "xno"], [
    WANT_USB=1
])

AC_ARG_ENABLE([pickit2], AS_HELP_STRING([--disable-pickit2], [Disable PICkit2 adapter]))
AS_IF([test "x$enable_pickit2" != "xno"], [
    WANT_USB=1
    WANT_HID=1
])

AC_ARG_ENABLE([uhb], AS_HELP_STRING([--disable-uhb], [Disable UHB adapter]))
AS_IF([test "x$enable_uhb" != "xno"], [
    WANT_USB=1
    WANT_HID=1
])

AS_IF([test "x$enable_an1388_uart" != "xno"], [
    WANT_SER=1
    SER_SRC='serial.c'
    SER_OBJ='serial.$(OBJEXT)'
    AC_DEFINE([ENABLE_SERIAL],[1],[Serial devices enabled])
])
AS_IF([test "x$enable_bitbang" != "xno"], [
    WANT_SER=1
    SER_SRC='serial.c'
    SER_OBJ='serial.$(OBJEXT)'
    AC_DEFINE([ENABLE_SERIAL],[1],[Serial devices enabled])
])
AS_IF([test "x$enable_stk500v2" != "xno"], [
    WANT_SER=1
    SER_SRC='serial.c'
    SER_OBJ='serial.$(OBJEXT)'
    AC_DEFINE([ENABLE_SERIAL],[1],[Serial devices enabled])
])

case $host_os in
    darwin* )
        AM_CONDITIONAL([OSX], true)
        AM_CONDITIONAL([LINUX], false)
        AM_CONDITIONAL([WINDOWS], false)
        AM_CONDITIONAL([BSD], false)
        AS_IF([test "x$WANT_USB" = "x1"], [
            LIBUSB_PREFIX=`pkg-config --libs-only-L libusb-1.0 | cut -c3-`
            AC_SUBST([LIBUSB_PREFIX])
            PKG_CHECK_MODULES(LIBUSB, libusb-1.0,[
                LIBUSB_LIBS="-framework IOKit -framework CoreFoundation ${LIBUSB_PREFIX}/libusb-1.0.a -lobjc"
            ],[WANT_USB=-1; WANT_HID=0])
        ])
        AS_IF([test "x$WANT_HID" = "x1"], [
            HID_SRC="hid/mac/hid.c"
            HID_OBJ='hid/mac/hid.$(OBJEXT)'
        ])

        OPT_ARCH=""

        AC_ARG_ENABLE([32bit],
            AS_HELP_STRING([--disable-32bit], [Disable 32-bit universal arch (OS X only)]))
        AC_ARG_ENABLE([64bit],
            AS_HELP_STRING([--disable-64bit], [Disable 64-bit universal arch (OS X only)]))
        AC_ARG_ENABLE([ppc],
            AS_HELP_STRING([--enable-ppc], [Enable PowerPC universal arch (OS X only)]))

        AS_IF([test "x$enable_32bit" != "xno"], [
            OPT_ARCH="${OPT_ARCH} -arch i386"
        ])
        AS_IF([test "x$enable_64bit" != "xno"], [
            OPT_ARCH="${OPT_ARCH} -arch x86_64"
        ])
        AS_IF([test "x$enable_ppc" = "xyes"], [
            OPT_ARCH="${OPT_ARCH} -arch ppc"
        ])

        AC_SUBST([OPT_ARCH])

        ;;
    linux* )
        AM_CONDITIONAL([OSX], false)
        AM_CONDITIONAL([LINUX], true)
        AM_CONDITIONAL([WINDOWS], false)
        AM_CONDITIONAL([BSD], false)
        AX_PTHREAD
        AS_IF([test "x$WANT_USB" = "x1"], [
            PKG_CHECK_MODULES(LIBUSB, libusb-1.0,[
                LIBUSB_STATIC='-Wl,-Bstatic -Wl,-lusb-1.0 -Wl,-Bdynamic'
            ],[WANT_USB=-1; WANT_HID=0; LIBUSB_STATIC=''])
            PKG_CHECK_MODULES(UDEV, libudev,,[WANT_USB=-1; WANT_HID=0; LIBUSB_STATIC=''])
        ])
        AS_IF([test "x$WANT_HID" = "x1"], [
            HID_SRC="hid/linux/hid.c"
            HID_OBJ='hid/linux/hid.$(OBJEXT)'
        ])
        ;;
    *bsd* )
        AM_CONDITIONAL([OSX], false)
        AM_CONDITIONAL([LINUX], true)
        AM_CONDITIONAL([WINDOWS], false)
        AM_CONDITIONAL([BSD], true)
        AX_PTHREAD
        AS_IF([test "x$WANT_USB" = "x1"], [
            LIBUSB_LIBS="-lusb"
        ])
        AS_IF([test "x$WANT_HID" = "x1"], [
            HID_SRC="hid/freebsd/hid.c"
            HID_OBJ='hid/freebsd/hid.$(OBJEXT)'
        ])
        ;;
    *mingw* )
        AM_CONDITIONAL([OSX], false)
        AM_CONDITIONAL([LINUX], false)
        AM_CONDITIONAL([WINDOWS], true)
        AM_CONDITIONAL([BSD], false)
        AS_IF([test "x$WANT_HID" = "x1"], [
            HID_SRC="hid/linux/hid.c"
            HID_OBJ='hid/linux/hid.$(OBJEXT)'
        ])
        AS_IF([test "x$WANT_USB" = "x1"], [
            LIBUSB_LIBS="-Llibusb-win32 -lusb-1.0 -lsetupapi"
            LIBUSB_CFLAGS="-Ilibusb-win32/libusb-1.0"
        ])
        AS_IF([test "x$WANT_HID" = "x1"], [
            HID_SRC="hid/windows/hid.c"
            HID_OBJ='hid/windows/hid.$(OBJEXT)'
        ])
        ;;
esac

# Second round decides what we can actually use.

AS_IF([test "x$enable_an1388" != "xno"], [
    AS_IF([test "x$WANT_USB$WANT_HID" = "x11"], [
        ADAPTER_SRC="${ADAPTER_SRC} adapters/adapter-an1388.c"
        ADAPTER_OBJ="${ADAPTER_OBJ} adapters/adapter-an1388.\$(OBJEXT)"
        AC_DEFINE([ENABLE_AN1388],[1],[AN1388 HID Adapter is enabled])
        WITH_AN1388=1
    ],)
])

AS_IF([test "x$enable_an1388_uart" != "xno"], [
    ADAPTER_SRC="${ADAPTER_SRC} adapters/adapter-an1388-uart.c"
    ADAPTER_OBJ="${ADAPTER_OBJ} adapters/adapter-an1388-uart.\$(OBJEXT)"
    AC_DEFINE([ENABLE_AN1388_UART],[1],[AN1388 UART Adapter is enabled])
    WITH_AN1388_UART=1
])

AS_IF([test "x$enable_bitbang" != "xno"], [
    ADAPTER_SRC="${ADAPTER_SRC} adapters/adapter-bitbang.c"
    ADAPTER_OBJ="${ADAPTER_OBJ} adapters/adapter-bitbang.\$(OBJEXT)"
    AC_DEFINE([ENABLE_BITBANG],[1],[BitBang Adapter is enabled])
    WITH_BITBANG=1
])

AS_IF([test "x$enable_hidboot" != "xno"], [
    AS_IF([test "x$WANT_USB$WANT_HID" = "x11"], [
        ADAPTER_SRC="${ADAPTER_SRC} adapters/adapter-hidboot.c"
        ADAPTER_OBJ="${ADAPTER_OBJ} adapters/adapter-hidboot.\$(OBJEXT)"
        AC_DEFINE([ENABLE_HIDBOOT],[1],[HidBoot Adapter is enabled])
        WITH_HIDBOOT=1
    ],)
])

AS_IF([test "x$enable_mpsse" != "xno"], [
    AS_IF([test "x$WANT_USB" = "x1"], [
        ADAPTER_SRC="${ADAPTER_SRC} adapters/adapter-mpsse.c"
        ADAPTER_OBJ="${ADAPTER_OBJ} adapters/adapter-mpsse.\$(OBJEXT)"
        AC_DEFINE([ENABLE_MPSSE],[1],[MPSSE Adapter is enabled])
        WITH_MPSSE=1
    ],)
])

AS_IF([test "x$enable_pickit2" != "xno"], [
    AS_IF([test "x$WANT_USB$WANT_HID" = "x11"], [
        ADAPTER_SRC="${ADAPTER_SRC} adapters/adapter-pickit2.c"
        ADAPTER_OBJ="${ADAPTER_OBJ} adapters/adapter-pickit2.\$(OBJEXT)"
        AC_DEFINE([ENABLE_PICKIT2],[1],[PICKIT2 Adapter is enabled])
        WITH_PICKIT2=1
    ],)
])

AS_IF([test "x$enable_stk500v2" != "xno"], [
    ADAPTER_SRC="${ADAPTER_SRC} adapters/adapter-stk500v2.c"
    ADAPTER_OBJ="${ADAPTER_OBJ} adapters/adapter-stk500v2.\$(OBJEXT)"
    AC_DEFINE([ENABLE_STK500V2],[1],[STK500v2 Adapter is enabled])
    WITH_STK500V2=1
])

AS_IF([test "x$enable_uhb" != "xno"], [
    AS_IF([test "x$WANT_USB$WANT_HID" = "x11"], [
        ADAPTER_SRC="${ADAPTER_SRC} adapters/adapter-uhb.c"
        ADAPTER_OBJ="${ADAPTER_OBJ} adapters/adapter-uhb.\$(OBJEXT)"
        AC_DEFINE([ENABLE_UHB],[1],[UHB Adapter is enabled])
        WITH_UHB=1
    ],)
])


if [[ -d .git ]]; then
    GITCOUNT=`git rev-list HEAD --count`
else
    if [[ -f .version ]]; then
        GITCOUNT=`cat .version`
    else
        GITCOUNT=0-detatched
    fi
fi

AC_SUBST([GITCOUNT])

AC_SUBST([SER_SRC])
AC_SUBST([SER_OBJ])
AC_SUBST([HID_SRC])
AC_SUBST([HID_OBJ])
AC_SUBST([ADAPTER_SRC])
AC_SUBST([ADAPTER_OBJ])
AC_SUBST([LIBUSB_LIBS])
AC_SUBST([LIBUSB_CFLAGS])
AC_SUBST([LIBUSB_STATIC])
AC_SUBST([UDEV_LIBS])
AC_SUBST([UDEV_CFLAGS])

CFLAGS="$CFLAGS $OPT_ARCH -Iinclude $LIBUSB_CFLAGS $UDEV_CFLAGS $PTHREAD_CFLAGS"
# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h locale.h malloc.h stdlib.h string.h sys/ioctl.h sys/time.h termios.h unistd.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([atexit clock_gettime gettimeofday memset select setlocale strcasecmp strchr strdup strerror strncasecmp strrchr strstr strtol strtoul])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

echo
echo "    Configuration Summary"
echo "    ---------------------"
echo
printf  "    AN1388 (HID) ............. "
AS_IF([test "x$WITH_AN1388" = "x1"], [echo Enabled], [echo Disabled])

printf  "    AN1388 (UART) ............ "
AS_IF([test "x$WITH_AN1388_UART" = "x1"], [echo Enabled], [echo Disabled])

printf  "    BitBang .................. "
AS_IF([test "x$WITH_BITBANG" = "x1"], [echo Enabled], [echo Disabled])

printf  "    HIDBoot .................. "
AS_IF([test "x$WITH_HIDBOOT" = "x1"], [echo Enabled], [echo Disabled])

printf  "    MPSSE .................... "
AS_IF([test "x$WITH_MPSSE" = "x1"], [echo Enabled], [echo Disabled])

printf  "    PICkit2/3 ................ "
AS_IF([test "x$WITH_PICKIT2" = "x1"], [echo Enabled], [echo Disabled])

printf  "    STK500v2 ................. "
AS_IF([test "x$WITH_STK500V2" = "x1"], [echo Enabled], [echo Disabled])

printf  "    UHB ...................... "
AS_IF([test "x$WITH_UHB" = "x1"], [echo Enabled], [echo Disabled])

echo
printf  "    USB Support .............. "
AS_IF([test "x$WANT_USB" = "x1"], [echo Enabled], [echo Disabled])

printf  "    HID Support .............. "
AS_IF([test "x$WANT_HID" = "x1"], [echo Enabled], [echo Disabled])

printf  "    Serial Support ........... "
AS_IF([test "x$WANT_SER" = "x1"], [echo Enabled], [echo Disabled])

echo

echo    "    Compiling For ............ $host_os"
echo

AS_IF([test "x$WANT_USB" = "x-1"], [
    echo "Warning:"
    echo "    USB support could not be enabled. You may need to install libusb-1.0"
    echo
],);
