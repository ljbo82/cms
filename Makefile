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

PROJ_NAME  := cms
PROJ_TYPE  := lib

# ------------------------------------------------------------------------------
include make/arduino/gcc-project/native_host.mk
include make/arduino/gcc-project/functions.mk
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
ifeq ($(HOST), )
    hostOS   := $(nativeOS)
    hostArch := $(nativeArch)
else
    ifeq ($(call fn_host_valid, $(HOST)), 0)
        $(error Invalid HOST: $(HOST))
    endif
    hostOS   := $(call fn_host_os, $(HOST))
    hostArch := $(call fn_host_arch, $(HOST))
endif
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
ifeq ($(wildcard hosts/$(HOST).mk), )
    ifeq ($(wildcard hosts/$(hostOS).mk), )
        $(error Unsupported HOST: $(HOST))
    else
        host_mk := hosts/$(hostOS).mk
    endif
else
    host_mk := hosts/$(HOST).mk
endif
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
include $(host_mk)
# ------------------------------------------------------------------------------

# ==============================================================================
.PHONY: clean-all
clean-all: clean
	@$(MAKE) -C demo/arduino clean
	@$(MAKE) -C demo/linux clean
# ==============================================================================

