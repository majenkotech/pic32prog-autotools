bin_PROGRAMS=pic32prog
pic32prog_LDADD=$(ADAPTER_OBJ) $(HID_OBJ) $(SER_OBJ)
pic32prog_DEPENDENCIES=$(ADAPTER_OBJ) $(HID_OBJ) $(SER_OBJ)
pic32prog_SOURCES=pic32prog.c configure.c executive.c target.c families/family-mz.c families/family-mx1.c families/family-mx3.c 
pic32prog_CFLAGS=-DGITCOUNT='"$(GITCOUNT)"' 
EXTRA_pic32prog_SOURCES=hid/linux/hid.c hid/mac/hid.c hid/windows/hid.c hid/freebsd/hid.c adapters/adapter-an1388.c adapters/adapter-an1388-uart.c adapters/adapter-bitbang.c adapters/adapter-hidboot.c adapters/adapter-mpsse.c adapters/adapter-pickit2.c adapters/adapter-stk500v2.c adapters/adapter-uhb.c serial.c

if BSD
pic32prog_LDADD+=$(LIBUSB_LIBS) $(PTHREAD_LIBS)
endif

if LINUX
pic32prog_LDFLAGS=-Wl,-start-group $(LIBUSB_STATIC)
pic32prog_LDADD+=$(UDEV_LIBS) $(PTHREAD_LIBS)
endif

if WINDOWS
pic32prog_CFLAGS+=-DMINGW32
pic32prog_LDADD+=$(LIBUSB_LIBS)
endif

if OSX
pic32prog_LDFLAGS=$(LIBUSB_LIBS)
endif
