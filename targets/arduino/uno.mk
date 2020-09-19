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

GCC_PREFIX := avr-

ifneq ($(wildcard ../arduino-avr-core-builder/core-src/cores/arduino), )
    ARDUINO_CORE_AVR_PATH ?= ../arduino-avr-core-builder/core-src
    ifeq ($(shell sh -c "cd $(ARDUINO_CORE_AVR_PATH) && git describe --tags > /dev/null 2>&1 && echo $$?"), 0)
        ARDUINO_CORE_AVR_VERSION := $(shell cd $(ARDUINO_CORE_AVR_PATH) && git describe --tags)
    endif
endif

ifeq ($(ARDUINO_CORE_AVR_PATH), )
    $(error Missing ARDUINO_CORE_AVR_PATH)
endif

ifeq ($(ARDUINO_CORE_AVR_VERSION), )
    $(error Missing ARDUINO_CORE_AVR_VERSION)
endif

__build_mcu__   := atmega328p
__build_f_cpu__ := 16000000L
__build_board__ := AVR_UNO
__build_arch__  := AVR
__variant__     := standard
__includes__    := -I$(ARDUINO_CORE_AVR_PATH)/variants/$(__variant__) -I$(ARDUINO_CORE_AVR_PATH)/cores

EXTRA_CFLAGS := -mmcu=$(__build_mcu__) -DF_CPU=$(__build_f_cpu__) -DARDUINO=$(ARDUINO_CORE_AVR_VERSION) -DARDUINO_$(__build_board__) -DARDUINO_ARCH_$(__build_arch__) $(__includes__)
undefine __includes__
