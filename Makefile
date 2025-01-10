######################################################################
#  Project
######################################################################

BINARY		= variable-flashing-rate
SRCFILES	= variable-flashing-rate.c

# src files from static libs
SRCFILES	+= lib/src/led.c
SRCFILES	+= lib/src/button.c
SRCFILES	+= lib/src/clock.c  

# HEADER FILES DIR
TGT_CFLAGS		+= -I$(TOP_DIR)/police-flashing-lights/include
TGT_CPPFLAGS	+= -I$(TOP_DIR)/police-flashing-lights/include

# DEPENDENCIES (STATIC LIBs)
LDLIBS			+= -L$(TOP_DIR)/library/lib



all: elf bin

include ../Makefile.incl

# End
