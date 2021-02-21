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

SRC_DIRS     += hosts/arduino
CORE_VERSION ?= 1.8.3
VARIANT      ?= uno
BUILD_BASE   := $(BUILD_BASE)/arduino

arduinoProjectMk := arduino-project.mk

ifeq ($(VARIANT), )
    $(error Missing VARIANT)
endif

ifneq ($(wildcard hosts/arduino/$(VARIANT).mk), )
    include hosts/arduino/$(VARIANT).mk
else
    $(error Unsupported VARIANT for arduino: $(VARIANT))
endif

ifeq ($(ARDUINO_AVR_CORE_BUILDER_PATH), )
    # Tries to find arduino-avr-core-builder automatically
    ifneq ($(wildcard ../arduino-avr-core-builder/$(arduinoProjectMk)), )
        ARDUINO_AVR_CORE_BUILDER_PATH := ../arduino-avr-core-builder
    endif
endif

ifeq ($(ARDUINO_AVR_CORE_BUILDER_PATH), )
    $(error ARDUINO_AVR_CORE_BUILDER_PATH not set/found)
endif

ifeq ($(wildcard $(ARDUINO_AVR_CORE_BUILDER_PATH)/$(arduinoProjectMk)), )
    $(error $(arduinoProjectMk) not found in ARDUINO_AVR_CORE_BUILDER_PATH ($(ARDUINO_AVR_CORE_BUILDER_PATH))}) 
endif

# Required since #{arduinoProjectMk} uses VARIANT variable
override undefine VARIANT

include $(ARDUINO_AVR_CORE_BUILDER_PATH)/$(arduinoProjectMk)

.PHONY: show-variants
show-variants:
	@find hosts/arduino -maxdepth 1 -type f ! -name arduino.mk | grep hosts/arduino/ | xargs -I{} basename {} .mk