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

GCC_PREFIX := avr
SRC_DIRS += hosts/arduino

ifneq ($(wildcard ../../../arduino-avr-core-builder/core-src/cores/arduino), )
    ARDUINO_CORE_AVR_PATH ?= ../../../arduino-avr-core-builder/core-src
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

build_mcu   := atmega328p
build_f_cpu := 16000000L
build_board := AVR_UNO
build_arch  := AVR
variant     := standard

INCLUDE_DIRS += $(ARDUINO_CORE_AVR_PATH)/variants/$(variant)
INCLUDE_DIRS += $(ARDUINO_CORE_AVR_PATH)/cores

override CFLAGS += -mmcu=$(build_mcu) -DF_CPU=$(build_f_cpu) -DARDUINO=$(ARDUINO_CORE_AVR_VERSION) -DARDUINO_$(build_board) -DARDUINO_ARCH_$(build_arch)
