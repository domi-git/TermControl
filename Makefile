#---------------------------------------
# Author: Dominik Muth
# Date: 16.02.19
#
# Automatic Variables:
# --------------------
# $@ - Target
# $^ - All Prerequisites
# $< - First Prerequisite
#---------------------------------------

#----------------------------------------------------------------------
# Einstellungen
#----------------------------------------------------------------------

# Verzeichnisse
BINDIR=./
SRCDIR=../

# Name resultierenden Binary-Files
BINNAME=TermControl 

# Compiler
CC=gcc

# Compiler-Flags
CFLAGS=`pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0 gmodule-2.0` 

# Linker-Flags
# lm = Link against math-library
LDFLAGS=-lm

# Source-Files
SRCS=main.c \
     serial.c

# Generate Object-Files from Source-Files
OBJS:=$(SRCS:.c=.o)

#----------------------------------------------------------------------
# Targets
#----------------------------------------------------------------------
.PHONY: all
all: $(OBJS) $(SUFILE_OBJ)
	@echo build $(BINNAME)
	@$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(SUFILE_OBJ) -o $(BINDIR)$(BINNAME)
	ctags -R . /usr/include/gtk-3.0/ /usr/include/glib-2.0/ /usr/include/termios.h

%.o: $(SRCDIR)%.c
	@echo compile $@
	@$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -o $(BINDIR)$@ 

.PHONY: clean
clean:
	rm -f *.o
