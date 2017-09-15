TOP=../../..
include $(TOP)/build/makefiles/MakeMacros.pre

SUB=ast
SOBJS=${OBJ}/$(SUB)/code.o \
	${OBJ}/$(SUB)/comment.o \
	${OBJ}/$(SUB)/error.o \
	${OBJ}/$(SUB)/delimiter.o \
	${OBJ}/$(SUB)/function.o \
	${OBJ}/$(SUB)/n.o \
	${OBJ}/$(SUB)/op.o \
	${OBJ}/$(SUB)/program.o \
	${OBJ}/$(SUB)/statement.o \
	${OBJ}/$(SUB)/type.o \
	${OBJ}/$(SUB)/variable.o 

TARGET=$(OBJ)/$(SUB)/libast.a
DEBUG_TARGET=$(OBJ)/$(SUB)/libast.debug.a
MEMSAN_TARGET=$(OBJ)/$(SUB)/libast.memsan.a
UBSAN_TARGET=$(OBJ)/$(SUB)/libast.ubsan.a
SCAN_TARGET=$(OBJ)/$(SUB)/libast.scan.a
all: dirs $(TARGET) $(DEBUG_TARGET) $(MEMSAN_TARGET) $(SCAN_TARGET) $(UBSAN_TARGET) 

include $(TOP)/build/makefiles/MakeMacros

