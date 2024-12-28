######################################################################
#  Project
######################################################################

BINARY		= main
SRCFILES	= main.c 

# HEADER FILES DIR
TGT_CFLAGS		+= -I$(TOP_DIR)/police-flashing-lights
TGT_CPPFLAGS	+= -I$(TOP_DIR)/police-flashing-lights


# DEPENDENCIES (STATIC LIBs)
#LDLIBS			+= -L$(TOP_DIR)/library/lib



all: elf bin

include ../Makefile.incl

# End
