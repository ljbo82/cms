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
LIB_TYPE   := static

# ------------------------------------------------------------------------------
# Both arduino and linux hosts are supported
ENFORCE_ARDUINO_ONLY := 0

# Since builder is 'arduino-gcc-builder', default search directory is 'boards'
HOSTS_DIR := hosts

# Buil only for hosts with an acceptable makefile in 'hosts' directory
HOST_MK_REQUIRED := 1

include make/project.mk
# ------------------------------------------------------------------------------

# ==============================================================================
.PHONY: clean-all
clean-all: clean
	@$(MAKE) -C demo/arduino clean
	@$(MAKE) -C demo/linux clean
# ==============================================================================

