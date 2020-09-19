# Copyright 2020 Leandro José Britto de Oliveira
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

# Debug build
DEBUG ?= 0
ifneq ($(DEBUG), 0)
    ifneq ($(DEBUG), 1)
        $(error Invalid value for DEBUG: $(DEBUG))
    endif
endif

# Verbosity
V ?= 0
ifneq ($(V), 0)
    ifneq ($(V), 1)
        $(error ERROR: Invalid value for V: $(V))
    endif
endif

ifeq ($(V), 0)
    __v__ := @
    __nl__ :=
else
    __v__ :=
    __nl__ := \n
endif

ifeq ($(wildcard targets/$(TARGET)), )
    $(error Unsupported TARGET: $(TARGET))
endif

ifneq ($(VARIANT), )
    ifneq ($(wildcard targets/$(TARGET)/$(VARIANT).mk), )
        include targets/$(TARGET)/$(VARIANT).mk
    else
        $(error Unsupported VARIANT for $(TARGET): $(VARIANT))
    endif
else
    ifneq ($(wildcard targets/$(TARGET)/$(TARGET).mk), )
        include targets/$(TARGET)/$(TARGET).mk
    endif
endif

ifneq ($(VARIANT), )
    __build_dir__ := build-$(TARGET)-$(VARIANT)
else
    __build_dir__ := build-$(TARGET)
endif

ifeq ($(DEBUG), 0)
    __build_artifact__ := libcms0.a
else
    __build_artifact__ := libcms0_d.a
    __build_dir__      := $(__build_dir__)-debug
endif

__include_dirs__ += targets/$(TARGET)
__include_dirs__ += src
__include_dirs__ += include
__src_dirs__     += targets/$(TARGET)
__src_dirs__     += src

__src_files__ := $(foreach srcDir, $(__src_dirs__), $(shell find $(srcDir) -name *.c))
__includes__  := $(foreach incDir, $(__include_dirs__), -I$(incDir))
__mkdir__     := mkdir -p
__obj_files__ := $(__src_files__:%=$(__build_dir__)/%.o)
__deps__      := $(__obj_files__:.o=.d)
__arflags__   := rcs

CC := $(GCC_PREFIX)gcc
AR := $(GCC_PREFIX)ar

__cflags__ += -c

ifeq ($(DEBUG), 1)
    __cflags__ += -g3
else
    __cflags__ += -Os
endif

__cflags__ += -Wall -std=gnu11 -ffunction-sections -fdata-sections -MMD -MP

.PHONY: bin clean clean-all

# Creates binaries for an specific target/variant
bin: $(__build_dir__)/$(__build_artifact__)

# Cleans binaries for an specific target/variant
clean:
    ifeq ($(TARGET), )
	    $(error Missing TARGET)
    endif
	$(__v__)rm -rf $(__build_dir__)

# Cleans any binary
clean-all:
	$(__v__)rm -rf build-*

# build artifact
$(__build_dir__)/$(__build_artifact__): $(__obj_files__)
	@printf "$(__nl__)[AR] $(__obj_files__)\n"
	$(__v__)$(AR) $(__arflags__) $@ $(__obj_files__) > /dev/null

# C files
$(__build_dir__)/%.c.o: %.c
    ifeq ($(TARGET), )
	    $(error Missing TARGET)
    endif
	@$(__mkdir__) $(dir $@)
	@printf "$(__nl__)[CC] $<\n"
	$(__v__)$(CC) $(strip $(__cflags__) $(EXTRA_CFLAGS) $(__includes__)) $< -o $@

-include $(__deps__)
