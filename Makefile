DEBUG ?= FALSE

# Toolchain
AS = nspire-as
GCC := nspire-gcc
GXX := nspire-g++
LD := $(GXX)
GENZEHN := genzehn
NDLESS_SDK ?= /home/jeremy/Ndless/ndless-sdk

# Compiler flags
GCCFLAGS := -Wall -Wextra -Wshadow -Wpedantic -marm #-fno-exceptions -fno-rtti -ffunction-sections -fdata-sections -Wl,--gc-sections
GCCFLAGS += -I$(NDLESS_SDK)/include # Include Ndless headers
ifeq ($(DEBUG),FALSE)
	GCCFLAGS += -Os
else
	GCCFLAGS += -O0 -g
endif
GXXFLAGS := $(GCCFLAGS) -std=c++17
LDFLAGS := -L$(NDLESS_SDK)/lib -lnspireio
USE_LCD_BLIT ?= true

ZEHNFLAGS := --name "displayTests"
ZEHNFLAGS += --uses-lcd-blit $(USE_LCD_BLIT)

# Object list: only .c and .cpp sources
OBJS := $(patsubst %.c, %.o, $(shell find . -name \*.c))
OBJS += $(patsubst %.cpp, %.o, $(shell find . -name \*.cpp))
OBJS += $(patsubst %.s, %.o, $(shell find . -name \*.s))

# File name
FILENAME := displayTests
BUILDDIR := build
OBJDIR := $(BUILDDIR)/obj
ELFDIR := $(BUILDDIR)/elf
BINDIR := $(BUILDDIR)/bin

DISTDIR := $(BINDIR)

vpath %.tns $(DISTDIR)
vpath %.elf $(DISTDIR)

all: $(BINDIR)/$(FILENAME).tns

############################
# Sources and objects
############################

SRCDIRS := src src/tools
CSRCS := $(foreach d,$(SRCDIRS),$(wildcard $(d)/*.c))
CPPSRCS := $(foreach d,$(SRCDIRS),$(wildcard $(d)/*.cpp))
ASMSRCS := $(foreach d,$(SRCDIRS),$(wildcard $(d)/*.s))

COBJS := $(addprefix $(OBJDIR)/,$(notdir $(CSRCS:.c=.o)))
CPPOBJS := $(addprefix $(OBJDIR)/,$(notdir $(CPPSRCS:.cpp=.o)))
ASMOBJS := $(addprefix $(OBJDIR)/,$(notdir $(ASMSRCS:.s=.o)))

OBJS := $(COBJS) $(CPPOBJS) $(ASMOBJS)

# vpath to locate sources
vpath %.c $(SRCDIRS)
vpath %.cpp $(SRCDIRS)
vpath %.s $(SRCDIRS)

############################
# Compile rules (preserve tree under obj/)
############################

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(GCC) $(GCCFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(GXX) $(GXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(GCC) $(GCCFLAGS) -c $< -o $@

# Link rule (use g++ so C++ runtime is linked)
$(ELFDIR)/$(FILENAME).elf: $(OBJS)
	@mkdir -p $(ELFDIR)
	$(LD) $(OBJS) -o $@ $(LDFLAGS)

# Package into .tns
$(BINDIR)/$(FILENAME).tns: $(ELFDIR)/$(FILENAME).elf
	@mkdir -p $(BINDIR)
	$(GENZEHN) --input $< --output $(ELFDIR)/$(FILENAME).zehn $(ZEHNFLAGS)
	make-prg $(ELFDIR)/$(FILENAME).zehn $@
	rm -f $(ELFDIR)/$(FILENAME).zehn

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)