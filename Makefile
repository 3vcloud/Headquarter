# Headquarter

CFLAGS	?= -O2 -pedantic
LDFLAGS ?= -pthread -ldl -lm
WARNING_CFLAGS ?= \
	-Wno-missing-field-initializers \
	-Wno-int-to-pointer-cast \
	-Wno-visibility

DFLAGS ?= -D_POSIX_C_SOURCE=200809L

DESTDIR=`/bin/pwd`/bin

.SILENT:
.PHONY: clean

LOCAL_CFLAGS = $(WARNING_CFLAGS) \
	-I$(DESTDIR)/include -Iinclude -Icode -Iinclude/linux

LOCAL_LDFLAGS = -L$(DESTDIR)/lib

client: code/client/main.c
	mkdir -p $(DESTDIR)
	$(MAKE) -C deps/mbedtls-2.16.5 install lib no_test DESTDIR=$(DESTDIR)
	$(CC) $(LOCAL_CFLAGS) code/client/main.c code/common/common.c $(DESTDIR)/lib/libmbedcrypto.a \
		-std=c11 $(DFLAGS) $(LOCAL_LDFLAGS) $(LDFLAGS) -o $(DESTDIR)/bin/client

.c.o:
	echo "  CC    $<"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -c $<

clean:
	rm -rf $(DESTDIR)
	$(MAKE) -C deps/mbedtls-2.16.5 clean

tests:
	echo $(DESTDIR)
