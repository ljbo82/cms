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

ifeq ($(HOST), )
    include make/arduino/gcc-project/native_host.mk
    hostOS   := $(nativeOS)
    hostArch := $(nativeArch)
else
    include make/arduino/gcc-project/functions.mk
    ifeq ($(call fn_host_valid, $(HOST)), 0)
        $(error Invalid HOST: $(HOST))
    endif
    hostOS   := $(shell echo $(HOST) | cut -d'-' -f1)
    hostArch := $(shell echo $(HOST) | cut -d'-' -f2-)
endif

ifeq ($(wildcard hosts/$(HOST).mk), )
    ifeq ($(wildcard hosts/$(hostOS).mk), )
        $(error Unsupported HOST: $(HOST))
    else
        include hosts/$(hostOS).mk
    endif
else
    include hosts/$(HOST).mk
endif

# ==============================================================================
.PHONY: clean-all
clean-all: clean
	@$(MAKE) -C demo/arduino clean
	@$(MAKE) -C demo/linux clean
# ==============================================================================

