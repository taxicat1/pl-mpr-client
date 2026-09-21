# Input paths
# -----------

SOURCEDIR       := src
ASMDIR          := asm
INCLUDEDIR      := include
LIBDIR          := lib
SYSINCLUDEDIRS  := $(LIBDIR)/include/MSL $(LIBDIR)/include/NitroSDK $(LIBDIR)/include/NitroSystem
FSDIR           := fs_local


# Configuration files
# -------------------

CRT0              := $(LIBDIR)/NitroSDK/crt0.o
LIBSYSCALL        := $(LIBDIR)/NitroSDK/libsyscall.a
RSF               := rom.rsf
LSF               := main.lsf
LCFTEMPLATE       := ARM9-TS.lcf.template
RESPONSETEMPLATE  := mwldarm.response.template
COMPARESHA        := rom.sha1

# Key to create the signature is unavailable, so use a prebuilt signature binary
SIG               := signature.bin


# Build artifacts
# ---------------

SRLNAME    := Rom-client.JPN.srl
BUILDDIR   := build
SRL        := $(BUILDDIR)/$(SRLNAME)
ELF        := $(BUILDDIR)/main.elf
SBIN       := $(ELF:.elf=.sbin)
SBINLZ     := $(SBIN)_LZ
LCF        := $(BUILDDIR)/$(LSF:.lsf=.lcf)
RESPONSE   := $(BUILDDIR)/$(LSF:.lsf=.response)
COMPONENT  := $(BUILDDIR)/component.files


# Wine (for MW/SDK binaries outside Windows)
# ------------------------------------------

ifeq ($(OS),Windows_NT)
  WINE := 
else
  WINE := wine
endif


# Tools
# -----

MWCCARMVER  := 2.0/sp2p2
MWCCARMDIR  := tools/mwccarm/$(MWCCARMVER)
MWCC        := $(WINE) $(MWCCARMDIR)/mwccarm.exe -lang=c99
MWCXX       := $(WINE) $(MWCCARMDIR)/mwccarm.exe -lang=c++
MWAS        := $(WINE) $(MWCCARMDIR)/mwasmarm.exe
MWLD        := $(WINE) $(MWCCARMDIR)/mwldarm.exe

SDKTOOLDIR  := tools/bin
ATTACHSIGN  := $(WINE) $(SDKTOOLDIR)/attachsign.exe
BIN2OBJ     := $(WINE) $(SDKTOOLDIR)/bin2obj.exe
MAKELCF     := $(WINE) $(SDKTOOLDIR)/makelcf.exe
COMPSTATIC  := $(WINE) $(SDKTOOLDIR)/compstatic.exe
MAKEROM     := $(WINE) $(SDKTOOLDIR)/makerom.exe

SED         := sed
CP          := cp
TOUCH       := touch
MKDIR       := mkdir
RM          := rm -rf
SHA1SUM     := sha1sum
FIND        := find


# Fix dependency directory separators
# -----------------------------------

define fixdep
  $(SED) -i 's/\r//g; s/\\/\//g; s/\/$$/\\/g' $(1)
  $(TOUCH) -r $(1:%.d=%.o) $(1)
endef


# Makefile flags
# --------------

COMPARE  ?= 1
VERBOSE  ?= 0

ifeq ($(VERBOSE),1)
  V :=
else
  V := @
endif


# Source files
# ------------

# No more asm
#SOURCES_S    := $(shell $(FIND) -L $(ASMDIR)    -type f -name "*.s")
SOURCES_S    := 

SOURCES_C    := $(shell $(FIND) -L $(SOURCEDIR) -type f -name "*.c")
SOURCES_CPP  := $(shell $(FIND) -L $(SOURCEDIR) -type f -name "*.cpp")
SOURCES_FS   := $(shell $(FIND) -L $(FSDIR)     -type f)


# Assembler/compiler/linker/tool flags
# ------------------------------------

DEFINES += -D_MSL_RESTRICT= -D_NITRO -DSDK_CW_FORCE_EXPORT_SUPPORT -DSDK_TS -DSDK_4M -DSDK_ARM9 -DSDK_CW -DSDK_FINALROM -DSDK_CODE_ARM -DNNS_FINALROM

INCLUDEFLAGS := -i $(INCLUDEDIR) $(foreach path,$(SYSINCLUDEDIRS),-I$(path))

ASFLAGS := -proc arm5te $(DEFINES) $(INCLUDEFLAGS)

MWCCFLAGS := -enum int -proc arm946e -gccext,on -fp soft -char signed -nosyspath -str reuse -inline on,noauto -Cpp_exceptions off -gccinc -interworking -ipa file -O4,p -stdinc -r -gccdep -MD -c -thumb \
           $(DEFINES) $(INCLUDEFLAGS)

LDFLAGS := -proc arm946e -dead -nostdlib -m _start -sym on -interworking -map unused -symtab sort -L$(LIBDIR)

COMPSTATICFLAGS := -9 -c

BIN2OBJFLAGS := -b FSLocal_%b_%e -e FSLocal_%b_%e_END


# Object files and dependencies
# -----------------------------

OBJS_SOURCES := $(addprefix $(BUILDDIR)/,$(addsuffix .o,$(SOURCES_S)))   \
                $(addprefix $(BUILDDIR)/,$(addsuffix .o,$(SOURCES_C)))   \
                $(addprefix $(BUILDDIR)/,$(addsuffix .o,$(SOURCES_CPP)))

DEPS := $(OBJS_SOURCES:.o=.d)

OBJS_FS := $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_FS)))

OBJS := $(OBJS_SOURCES) $(OBJS_FS)


# Targets
# -------

.PHONY: all srl clean

all: srl

srl: $(SRL)

clean:
	@echo "  CLEAN"
	$(V)$(RM) $(BUILDDIR)


# SRL rules
# ---------

$(SRL): $(OBJS) $(SBINLZ) $(RSF) $(SIG)
	@echo "  MAKEROM"
	$(V)$(MAKEROM) $(RSF) $@
	
	@echo "  ATTACHSIGN"
	$(V)$(ATTACHSIGN) $(SRL) $(SIG) $(SRL)
	
ifeq ($(COMPARE),1)
	@echo "  COMPARE"
	$(V)$(SHA1SUM) -c $(COMPARESHA)
endif

$(SBINLZ): $(ELF)
	@echo "  COMPSTATIC"
	$(V)$(COMPSTATIC) $(COMPSTATICFLAGS) -f $(COMPONENT)

$(ELF) $(COMPONENT): $(OBJS) $(LCF) $(RESPONSE) $(LIBSYSCALL) $(CRT0)
	@echo "  MWLDARM"
	$(V)$(MWLD) $(LDFLAGS) $(LCF) @$(RESPONSE) $(LIBSYSCALL) $(CRT0) -o $@

$(LCF): $(LSF) $(LCFTEMPLATE)
	@echo "  MAKELCF"
	$(V)$(MAKELCF) $^ $@

$(RESPONSE): $(LSF) $(RESPONSETEMPLATE)
	$(V)$(MAKELCF) $^ $@


# General rules for compilation/assembly/fs
# -----------------------------------------

$(BUILDDIR)/%.s.o : %.s
	@echo "  MWASMARM    $<"
	@$(MKDIR) -p $(@D)
	$(V)$(MWAS) $(ASFLAGS) $< -o $@

$(BUILDDIR)/%.c.o : %.c
	@echo "  MWCCARM     $<"
	@$(MKDIR) -p $(@D)
	$(V)$(MWCC) $(MWCCFLAGS) $< -o $@
	@$(call fixdep, $(@:.o=.d))

$(BUILDDIR)/%.cpp.o : %.cpp
	@echo "  MWCCARM     $<"
	@$(MKDIR) -p $(@D)
	$(V)$(MWCXX) $(MWCCFLAGS) $< -o $@
	@$(call fixdep, $(@:.o=.d))

$(BUILDDIR)/$(FSDIR)/%.o : $(FSDIR)/%
	@echo "  BIN2OBJ     $<"
	@$(MKDIR) -p $(@D)
	$(V)$(BIN2OBJ) $(BIN2OBJFLAGS) $< $@ 


# Dependency files if they exist
# ------------------------------

-include $(DEPS)
