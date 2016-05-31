#
#	Makefile
#

UNAME_S := $(shell uname -s)

CC := gcc

SRCDIR := .
LIBDIR := lib
BUILDDIR := build
TARGETDIR := bin
INSTALLBINDIR := /usr/local/bin
TARGET := bin/cryptobox

SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
# OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
OBJECTS := $(patsubst %.c, %.c.o, $(SOURCES))

CFLAGS := -c -ggdb

LIB := -L ../bitpunch/lib/dist -lmbedcrypto -lbpumecs
INC := -I ../bitpunch/lib/src -I ./

$(TARGET): $(OBJECTS)
	@mkdir -p $(TARGETDIR)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB);
	if [ -L asn1 ] ; then echo "ASN1 symlink exists"; else ln -s ../bitpunch/lib/asn1/ asn1;fi
	if [ -L dist ] ; then echo "DIST symlink exists"; else ln -s ../bitpunch/lib/dist dist;fi

%.c.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	find ./ -type f -name '*.o' -exec rm -v {} \;
	find ./ -type f -name '*.pic' -exec rm -v {} \;
	find ./ -type f -name '*.gch' -exec rm -v {} \;

install:
	@echo " Installing...";
	@echo " cp $(TARGET) $(INSTALLBINDIR)"; cp $(TARGET) $(INSTALLBINDIR)
	

.PHONY: clean
