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

PROJ_NAME := cms0
PROJ_TYPE := lib
LIB_TYPE  ?= static
PRE_BUILD ?= $(if $(HOST),,@echo Missing HOST; exit 1)

ifneq ($(HOST),)
    ifeq ($(wildcard hosts/$(HOST)/$(HOST).mk), )
        $(error Unsupported HOST: $(HOST))
    else
        include hosts/$(HOST)/$(HOST).mk
    endif
endif

CFLAGS += -std=gnu11 -ffunction-sections -fdata-sections

include c-cpp-project.mk

.PHONY: show-hosts
show-hosts:
	@echo linux
	@echo arduino
