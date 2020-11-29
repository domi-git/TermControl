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

# Directories
BINDIR=./
SRCDIR=../

# Name of resulting binary files
BINNAME=TermControl 

# Compiler
CC=gcc

# Compiler-Flags
CFLAGS=`pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0 gmodule-2.0` 

# Linker-Flags
# lm = Link against math-library
# lncurses = Link against ncurses library
LDFLAGS=-lm -lncurses -ltinfo

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
	@$(CC) $(CFLAGS) $(OBJS) $(SUFILE_OBJ) -o $(BINDIR)$(BINNAME) $(LDFLAGS)

%.o: $(SRCDIR)%.c
	@echo compile $@
	@$(CC) $(CFLAGS) -c $^ -o $(BINDIR)$@ $(LDFLAGS) 

.PHONY: clean
clean:
	rm -f *.o
