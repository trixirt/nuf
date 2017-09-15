TOP=../../..
include $(TOP)/build/makefiles/MakeMacros.pre

SUB=visitors
SOBJS=${OBJ}/$(SUB)/forth_emit.o

TARGET=$(OBJ)/$(SUB)/libvisitors.a
DEBUG_TARGET=$(OBJ)/$(SUB)/libvisitors.debug.a
MEMSAN_TARGET=$(OBJ)/$(SUB)/libvisitors.memsan.a
UBSAN_TARGET=$(OBJ)/$(SUB)/libvisitors.ubsan.a
SCAN_TARGET=$(OBJ)/$(SUB)/libvisitors.scan.a

INC+=$(LLVM_INC)
CXXFLAGS+=$(LLVM_CXXFLAGS)
all: dirs $(TARGET) $(DEBUG_TARGET) $(MEMSAN_TARGET) $(SCAN_TARGET) $(UBSAN_TARGET) 

include $(TOP)/build/makefiles/MakeMacros

