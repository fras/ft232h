# File: Makefile
# Auth: M. Fras, Electronics Division, MPI for Physics, Munich
# Mod.: M. Fras, Electronics Division, MPI for Physics, Munich
# Date: 05 Feb 2018
# Rev.: 09 Feb 2018
#
# Makefile for the I2C raw IO control using the FDTI FH232H chip.
#



# ********** Check on which OS we are compiling. **********
OS       = $(shell uname -s)



# ********** Program parameters. **********
PROG         = i2c-io
SOURCE_FILES = i2c-io.c

HEADER_FILES = i2c-io.h



# ********** Additional settings. **********
BACKUP_DIR         = backup
BACKUP_FILES_SRC   = $(SOURCE_FILES) $(HEADER_FILES) Makefile
RM_FILES_CLEAN     = core *.o *.stackdump $(PROG)
RM_FILES_REALCLEAN = $(RM_FILES_CLEAN) *.bak *~



# ********** Compiler configuration. **********
CROSS_COMPILE =
CC       = $(CROSS_COMPILE)gcc
CPP      = $(CC) -E
CXX      = $(CROSS_COMPILE)g++
#CFLAGS   = -O2 -Wall -I/usr/include/libftdi1 -I/usr/local/include/libftdi1
CFLAGS   = -O2 -Wall -I/usr/include/libftdi1 -I/usr/local/include/libftdi1 -I../libi2c_mpsse
CXXFLAGS = -O2 -Wall
LDFLAGS  =
INCLUDES = -I.
#LDLIBS   = -L. -L/usr/local/lib -l:libmpsse.a -lftdi1
LDLIBS   = -L. -L/usr/local/lib -L../libi2c_mpsse -l:libi2c_mpsse.a -l:libmpsse.a -lftdi1



# ********** Auxiliary programs, **********
BZIP2           = bzip2
CD              = cd
CP              = cp -a
CVS             = cvs
DATE            = date
DATE_BACKUP     = $(DATE) +"%Y-%m-%d_%H-%M-%S"
ECHO            = echo
ECHO_ERR        = $(ECHO) "**ERROR:"
EDIT			= gvim
EXIT            = exit
EXPORT          = export
FALSE           = false
GIT             = git
GREP            = grep
GZIP            = gzip
LN              = ln -s
MAKE            = make
MSGVIEW         = msgview
MV              = mv
SLEEP           = sleep
SH              = sh -c 
RM              = rm
TAIL            = tail -n 5
TAR             = tar
TCL             = tclsh
TEE             = tee
TOUCH           = touch
WISH            = wish



# ********** Generate object files variable. **********
OBJS := $(SOURCE_FILES:.c=.o)
OBJS := $(OBJS:.cc=.o)
OBJS := $(OBJS:.cpp=.o)
OBJS := $(OBJS:.C=.o)



# ********** Rules. **********
.PHONY: all exec edit install clean real_clean mrproper mk_backup mk_backup_src

all: $(PROG) install

exec: install
	./$(PROG)

install: $(PROG)
#	@-$(RM) ../bin/$(PROG)
#	@-$(RM) ../bin/$(PROG).exe
#	@-$(LN) ../src/$(PROG) ../bin/$(PROG)
#	@-$(LN) ../src/$(PROG) ../bin/$(PROG).exe

edit: $(SOURCE_FILES) $(HEADER_FILES)
	@$(EDIT) $(SOURCE_FILES) $(HEADER_FILES)

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) 

$(OBJS): $(HEADER_FILES)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

%.o: %.cc
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

%.o: %.C
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<



# ********** Check if all necessary files and dirctories are there. **********
$(SOURCE_FILES) $(HEADER_FILES):
	@$(ECHO_ERR) "Some source files are missing!"
	@$(ECHO) "Check:"
	@$(SH) 'for source_file in $(SOURCE_FILES) $(HEADER_FILES); do \
		if [ ! -e $$source_file ]; then \
			$(ECHO) $$source_file; \
		fi; \
	done'
	@$(FALSE)

$(BACKUP_DIR):
	@$(ECHO_ERR) "Backup directory is missing!"
	@$(ECHO) "Check:"
	@$(ECHO) "$(BACKUP_DIR)"



# ********** Create backup of current state. **********
mk_backup: mk_backup_src

mk_backup_src: $(BACKUP_DIR) $(SOURCE_FILES) $(HEADER_FILES)
	@$(SH) ' \
	backup_file=$(PROG)_src_`$(DATE_BACKUP)`.tgz; \
	$(EXPORT) backup_file; \
	$(TAR) cfz "$(BACKUP_DIR)/$$backup_file" $(BACKUP_FILES_SRC); \
	TAR_RETURN=$$?; \
	if [ ! $$TAR_RETURN = 0 ]; then \
		$(ECHO_ERR) "Error occured backing up files."; \
	fi; \
	if [ -f $(BACKUP_DIR)/$$backup_file ]; then \
		$(ECHO) "Created source file(s) backup \"$(BACKUP_DIR)/$$backup_file\"."; \
	else \
		$(ECHO_ERR) "Cannot create \"$(BACKUP_DIR)/$$backup_file\"."; \
	fi'



# ********** Tidy up. **********
clean:
	@$(SH) 'RM_FILES="$(RM_FILES_CLEAN)"; \
		$(EXPORT) RM_FILES; \
		$(ECHO) "Removing files: \"$$RM_FILES\""; \
		$(RM) $$RM_FILES 2> /dev/null; \
		$(ECHO) -n'

real_clean:
	@$(SH) 'RM_FILES="$(RM_FILES_REALCLEAN)"; \
		$(EXPORT) RM_FILES; \
		$(ECHO) "Removing files: \"$$RM_FILES\""; \
		$(RM) $$RM_FILES 2> /dev/null; \
		$(ECHO) -n'

mrproper: real_clean

