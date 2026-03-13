################################################################################
######################### User configurable parameters #########################
# These are the only values you should normally need to edit.

ROOT   = .
FWDIR  = $(ROOT)/firmware
BINDIR = $(ROOT)/bin
SRCDIR = $(ROOT)/src
INCDIR = $(ROOT)/include

CEXTS   := c
ASMEXTS := s S
CXXEXTS := cpp c++ cc

WARNFLAGS     +=
EXTRA_CFLAGS   =
EXTRA_CXXFLAGS =

USE_PACKAGE             := 1
EXCLUDE_COLD_LIBRARIES  :=

IS_LIBRARY := 1
LIBNAME    := Vortex
VERSION    := 1.0.0

TEMPLATE_FILES = \
    $(wildcard $(INCDIR)/Vortex/*.hpp)    \
    $(wildcard $(INCDIR)/Vortex/**/*.hpp) \
    LICENSE                                \
    README.md                             \
    template.pros

################################################################################
# common.mk is provided by the PROS kernel and handles all real build logic.
# It is downloaded automatically on first `pros make`, or install manually:
#   pros c fetch kernel && pros c install kernel
################################################################################
.DEFAULT_GOAL = quick

-include ./common.mk

# Build + package the distributable template ZIP
.PHONY: template
template:: clean-template $(LIBAR)
	$Dpros c create-template . $(LIBNAME) $(VERSION) \
		$(foreach file,$(TEMPLATE_FILES),--system "$(file)") \
		--system "$(LIBAR)" --target v5 $(CREATE_TEMPLATE_FLAGS)

# Shorthand target
.PHONY: library
library: $(LIBAR)

# Open PROS serial terminal
.PHONY: terminal
terminal:
	pros terminal
