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

PROJ_NAME  := cms
PROJ_TYPE  := lib
LIB_TYPE   ?= static

ARDUINO_BUILDER     ?= builder/arduino-builder
CPP_PROJECT_BUILDER ?= builder/cpp-project-builder

include $(ARDUINO_BUILDER)/layers.mk

# Since LIB_TYPE will be defined after platform layers are processed,
# includes project.mk instead of builder.mk in order to fully parse project
# data prior to compiler management.
include $(CPP_PROJECT_BUILDER)/project.mk

# At this point, LIB_TYPE is defined, but compiler management did not took
# place yet.

# Source code use special decorators when building a shared library
ifeq ($(LIB_TYPE),shared)
    CFLAGS += -DBUILD_SHARED_LIB
endif

include $(CPP_PROJECT_BUILDER)/builder.mk
