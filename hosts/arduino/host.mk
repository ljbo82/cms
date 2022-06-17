# Copyright (c) 2022 Leandro José Britto de Oliveira
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

ifeq ($(ARDUINO_CORE_DIR),)
    $(error [ARDUINO_CORE_DIR] Mising value)
endif

INCLUDE_DIRS += $(O_DIST_DIR)/include
LDFLAGS      += -L$(O_DIST_DIR)/lib -larduino-core1

PRE_BUILD_DEPS += arduino-core

# ==============================================================================
.PHONY: arduino-core
arduino-core:
    # NOTE: Uses a custom BUILD_DIR in order to isolate library object files from application ones.
	$(O_VERBOSE)$(MAKE) -C $(ARDUINO_CORE_DIR) BUILD_DIR=build/lib O=$(shell realpath -m --relative-to=$(ARDUINO_CORE_DIR) $(O))
# ==============================================================================