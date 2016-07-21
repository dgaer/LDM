#######################
#### Start of File ####
#######################
# ---------------------------------------------------------------
# Makefile
# C/C++ Compiler Used: gcc version 4.4 or higher
# ---------------------------------------------------------------
# Define a name for the executable
PROJECT = dev_queue
64BITCFG = 1
FINAL = 1

# Library paths
APP_PATH = ./
LDM := $(LDMHOME)
LDM_LIB_DIR := $(LDMHOME)/lib

# The CFLAGS must match your configure options
# GNU builds:
CC = gcc
CFLAGS = -Wall -std=gnu99 -DHAVE_CONFIG_H

include linux.env

PROJECT_DEP = dev_queue.h \
ldm_functions.h \
ldm_functions.c \
dev_functions.c \
file_utils.h \
file_utils.c \
m_config.h \
m_config.c \
m_help.h \
m_help.c \
m_log.h \
m_log.c \
m_proc.h \
m_proc.c

# Compile the files and build the executable
# ===============================================================
all:    $(PROJECT)

$(PROJECT)$(OBJ_EXT):	$(APP_PATH)$(PATHSEP)$(PROJECT).c $(PROJECT_DEP)
	$(CC) $(COMPILE_ONLY) $(COMPILE_FLAGS) $(APP_PATH)$(PATHSEP)$(PROJECT).c

# Make the executable
OBJECTS = $(PROJECT)$(OBJ_EXT)

$(PROJECT):	$(OBJECTS)
	$(CC) $(COMPILE_FLAGS) $(OBJECTS)  \
	$(OUTPUT) $(PROJECT) $(LINK_LIBRARIES) $(LIBRARIES) $(LINKER_FLAGS)

# ===============================================================
# Set to your installation directory
install:
	mkdir -vp $(LDMHOME)/util
	cp -fv $(PROJECT) $(LDMHOME)/util/$(PROJECT)
	chmod 755 $(LDMHOME)/util/$(PROJECT)

# Remove object files and the executable after running make
# ===============================================================
clean:
	echo Removing all OBJECT files from working directory...
	rm -f *.o

	echo Removing EXECUTABLE file from working directory...
	rm -f $(PROJECT)

	echo Removing all test LOG files from working directory...
	rm -f *.log

	echo Removing all test OUT files from working directory...
	rm -f *.out 

	echo Removing DEBUG file...
	rm -f *.expand
	rm -f *.sibling
	rm -f *.initvals
	rm -f *.unshare
	rm -f *.vregs
	rm -f *.into_cfglayout
	rm -f *.jump
	rm -f *.reginfo
	rm -f *.outof_cfglayout
	rm -f *.split1
	rm -f *.dfinit
	rm -f *.mode_sw
	rm -f *.asmcons
	rm -f *.subregs_of_mode_init
	rm -f *.ira
	rm -f *.subregs_of_mode_finish
	rm -f *.split2
	rm -f *.pro_and_epilogue
	rm -f *.stack
	rm -f *.alignments
	rm -f *.mach
	rm -f *.barriers
	rm -f *.eh_ranges
	rm -f *.shorten
	rm -f *.dfinish
	rm -f *.final
	rm -f *.eh

# ---------------------------------------------------------------
#####################
#### End of File ####
#####################
