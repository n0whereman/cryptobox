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

#LIB := -L /Users/momo/Desktop/Skolka/DIPLOMKA/bitpunch/lib/dist -lmbedcrypto -lbpumecs
LIB := -L ../bitpunch/lib/dist -lmbedcrypto -lbpumecs
#INC := -I /Users/momo/Desktop/Skolka/DIPLOMKA/bitpunch/lib/src -I ./
INC := -I ../bitpunch/lib/src -I ./

$(TARGET): $(OBJECTS)
	@mkdir -p $(TARGETDIR)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

# $(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
# 	@echo $(SRCDIR).$(SRCEXT)
# 	@mkdir -p $(BUILDDIR)
# 	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

%.c.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# clean:
# 	@echo " Cleaning...";
# 	@echo " $(RM) -rv $(BUILDDIR) $(TARGET)"; $(RM) -rv $(BUILDDIR) $(TARGET)

clean:
	find ./ -type f -name '*.o' -exec rm -v {} \;
	find ./ -type f -name '*.pic' -exec rm -v {} \;
	find ./ -type f -name '*.gch' -exec rm -v {} \;

install:
	@echo " Installing...";
	@echo " cp $(TARGET) $(INSTALLBINDIR)"; cp $(TARGET) $(INSTALLBINDIR)
	
distclean:
	@echo " Un-Installing...";
	@echo " rm /usr/local/bin/CoralSeaServer"; rm /usr/local/bin/CoralSeaServer

.PHONY: clean
