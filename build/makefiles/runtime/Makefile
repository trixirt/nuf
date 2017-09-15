TOP=../../..
include $(TOP)/build/makefiles/MakeMacros.pre

SUB=runtime
SOBJS=${OBJ}/$(SUB)/runtime.o

TARGET=$(OBJ)/$(SUB)/libnufrt.a
DEBUG_TARGET=$(OBJ)/$(SUB)/libnufrt.debug.a
INC+=-I$(TOP)/src

all: dirs $(TARGET) $(DEBUG_TARGET)

include $(TOP)/build/makefiles/MakeMacros

format :
	- $(FORMAT) -i ${SRC}/$(SUB)/*.c
