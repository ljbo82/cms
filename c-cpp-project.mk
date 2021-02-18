# Copyright 2021 Leandro José Britto de Oliveira
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

ifeq ($(PROJ_NAME),)
    $(error Missing PROJ_NAME)
endif

PROJ_TYPE ?= app
ifneq ($(PROJ_TYPE), app)
    ifneq ($(PROJ_TYPE), lib)
        $(error Unsupported PROJ_TYPE: $(PROJ_TYPE))
    else
    LIB_TYPE ?= shared
        ifneq ($(LIB_TYPE), shared)
            ifneq ($(LIB_TYPE), static)
                $(error Unsupported LIB_TYPE: $(LIB_TYPE))
            endif
        endif
    endif
endif

DEBUG ?= 0
ifneq ($(DEBUG), 0)
    ifneq ($(DEBUG), 1)
        $(error Invalid value for DEBUG: $(DEBUG))
    endif
endif

V ?= 0
ifneq ($(V), 0)
    ifneq ($(V), 1)
        $(error ERROR: Invalid value for V: $(V))
    endif
endif

ifeq ($(V), 0)
    v := @
    nl :=
else
    v :=
    nl := \n
endif

BUILD_DIR ?= build
ifneq ($(HOST), )
    buildDir := $(BUILD_DIR)/$(HOST)
else
    buildDir := $(BUILD_DIR)
endif

ifneq ($(wildcard src), )
    srcDirs += src
endif
srcDirs += $(SRC_DIRS)

ifneq ($(wildcard include), )
    includeDirs += include
endif
includeDirs += $(INCLUDE_DIRS)

ifeq ($(DEBUG), 1)
    objSuffix := .dbg.o
else
    objSuffix := .o
endif
srcFiles := $(foreach srcDir, $(srcDirs), $(shell find $(srcDir) -name *.c -or -name *.cpp -or -name *.s))
objFiles := $(srcFiles:%=$(buildDir)/%$(objSuffix))
deps := $(objFiles:.o=.d)

ifeq ($(DEBUG), 1)
    artifactNameSuffix := _d
endif
ifeq ($(PROJ_TYPE), lib)
    ARTIFACT_PREFIX ?= lib
    ifeq ($(LIB_TYPE), static)
        ARTIFACT_SUFFIX ?= .a
    else
        ARTIFACT_SUFFIX ?= .so
    endif
endif
artifactName := $(ARTIFACT_PREFIX)$(PROJ_NAME)$(artifactNameSuffix)$(ARTIFACT_SUFFIX)
buildArtifact := $(buildDir)/$(artifactName)

gccFlags += -Wall
ifeq ($(DEBUG), 1)
    gccFlags += -g3
else
    gccFlags += -O2
endif

gccFlags += $(foreach srcDir, $(srcDirs), -I$(srcDir))
gccFlags += $(foreach includeDir, $(includeDirs), -I$(includeDir))
ifeq ($(DEBUG), 1)
	gccFlags += -DDEBUG=1
endif
ifeq ($(PROJ_TYPE), lib)
    ifeq ($(LIB_TYPE), shared)
        gccFlags += -fPIC
    endif
endif
gccFlags += -MMD -MP

ifeq ($(DEBUG), 1)
    asFlags += --gstabs+
endif

ifeq ($(DEBUG), 0)
    ldFlags += -s
endif
ifeq ($(PROJ_TYPE), lib)
    ifeq ($(LIB_TYPE), shared)
        ldFlags += -shared
    endif
endif

arFlags += rcs

ifndef GCC_PREFIX
    ifneq ($(HOST), )
        gccPrefix := $(HOST)-
    endif
else
    gccPrefix := $(patsubst "%",%,$(GCC_PREFIX))
endif

LD  := $(CC)
CC  := $(gccPrefix)$(CC)
CXX := $(gccPrefix)$(CXX)
AS  := $(gccPrefix)$(AS)
AR  := $(gccPrefix)$(AR)
LD  := $(gccPrefix)$(LD)]

.PHONY: all
all: $(buildArtifact)

.PHONY: clean
clean:
	$(v)rm -rf $(buildDir)

.PHONY: pre-build
pre-build:
    ifneq ($(PRE_BUILD), )
	    $(v)$(PRE_BUILD)
    endif

.PHONY: post-build
    ifneq ($(POST_BUILD), )
	    $(v)$(POST_BUILD)
    endif

$(buildArtifact): pre-build $(objFiles) post-build
    ifeq ($(PROJ_TYPE), lib)
        ifeq ($(LIB_TYPE), shared)
	        @printf "$(nl)[LD] $(objFiles)\n"
	        $(v)$(LD) $(strip $(objFiles) -o $@ $(ldFlags) $(LDFLAGS))
        else
	        @printf "$(nl)[AR] $(objFiles)\n"
	        $(v)$(AR) $(strip $(arFlags) $@ $(objFiles))
        endif
    else
	    @printf "$(nl)[LD] $(objFiles)\n"
	    $(v)$(LD) $(strip $(objFiles) -o $@ $(ldFlags) $(LDFLAGS))
    endif

$(buildDir)/%.c$(objSuffix): %.c
	@mkdir -p $(dir $@)
	@printf "$(nl)[CC] $<\n"
	$(v)$(CC) $(strip $(gccFlags) $(CFLAGS) -c $< -o $@)

$(buildDir)/%.cpp$(objSuffix): %.cpp
	@mkdir -p $(dir $@)
	@printf "$(nl)[CXX] $<\n"
	$(v)$(CXX) $(strip $(gccFlags) $(CXXFLAGS) -c $< -o $@)

$(buildDir)/%.s$(objSuffix): %.s
	@mkdir -p $(dir $@)
	@printf "$(nl)[AS] $<\n"
	$(v)$(AS) $(strip $(asFlags) $(ASFLAGS) -c $< -o $@)

-include $(deps)