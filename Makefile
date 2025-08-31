# Makefile for pam_duress_fedora

# Compiler and flags
CC = gcc
CFLAGS = -fPIC -c
LDFLAGS = -shared
LIBS = -lpam

# Source and output files
SRC = pam_duress_fedora.c
OBJ = pam_duress_fedora.o
SO = pam_duress_fedora.so
INSTALL_DIR = /lib64/security/
INSTALL_PATH = $(INSTALL_DIR)$(SO)

# Default target
all: $(SO)

# Compile the object file
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OBJ)

# Create the shared library
$(SO): $(OBJ)
	$(CC) $(LDFLAGS) -o $(SO) $(OBJ) $(LIBS)

# Install the shared library
install: $(SO)
	@if [ -f $(INSTALL_PATH) ]; then \
		echo "Module already installed. Removing old version."; \
		rm -f $(INSTALL_PATH); \
	fi
	mv $(SO) $(INSTALL_DIR)
	chown root:root $(INSTALL_PATH)
	chmod 755 $(INSTALL_PATH)

# Clean up build files
clean:
	rm -f $(OBJ) $(SO)

# Phony targets
.PHONY: all install clean
