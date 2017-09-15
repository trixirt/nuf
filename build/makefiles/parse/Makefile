TOP=../../..
include $(TOP)/build/makefiles/MakeMacros.pre

SUB=parse
SOBJS=${OBJ}/$(SUB)/forth_p.o

TARGET=$(OBJ)/$(SUB)/libparse.a
DEBUG_TARGET=$(OBJ)/$(SUB)/libparse.debug.a
MEMSAN_TARGET=$(OBJ)/$(SUB)/libparse.memsan.a
UBSAN_TARGET=$(OBJ)/$(SUB)/libparse.ubsan.a
SCAN_TARGET=$(OBJ)/$(SUB)/libparse.scan.a
INC+=-I$(OBJ)
INC+=$(LLVM_INC)
CXXFLAGS+=$(LLVM_CXXFLAGS)

all: dirs $(TARGET) $(DEBUG_TARGET) $(MEMSAN_TARGET) $(SCAN_TARGET) $(UBSAN_TARGET) 

include $(TOP)/build/makefiles/MakeMacros

format :
	true

${OBJ}/$(SUB)/forth_p.cpp : $(SRC)/$(SUB)/forth.lem $(SRC)/$(SUB)/lempar.c
	$(LEMON) -T$(SRC)/$(SUB)/lempar.c $(SRC)/$(SUB)/forth.lem
	mv $(SRC)/$(SUB)/forth.h $(OBJ)/$(SUB)/forth_p.h
	mv $(SRC)/$(SUB)/forth.c $(OBJ)/$(SUB)/forth_p.cpp

${OBJ}/$(SUB)/forth_p.o : ${OBJ}/$(SUB)/forth_p.cpp
	$(CXX) -c $(CXXFLAGS) -I$(INC) $(DEFINES) $(OPTIMIZED_FLAGS) $(PROJECT_INCLUDE) $< -o $@

${OBJ}/$(SUB)/forth_p.debug.o : ${OBJ}/$(SUB)/forth_p.cpp
	$(CXX) -c $(CXXFLAGS) -I$(INC) $(DEFINES) $(PROJECT_INCLUDE) $(DEBUG_FLAGS) $< -o $@

${OBJ}/$(SUB)/forth_p.memsan.o : ${OBJ}/$(SUB)/forth_p.cpp
	$(CXX) -c $(CXXFLAGS) -I$(INC) $(DEFINES) $(PROJECT_INCLUDE) $(DEBUG_FLAGS) $(MEMSAN_FLAGS) $< -o $@

${OBJ}/$(SUB)/forth_p.scan.o : ${OBJ}/$(SUB)/forth_p.cpp
	$(SCAN)	$(CXX) -c $(CXXFLAGS) -I$(INC) $(DEFINES) $(PROJECT_INCLUDE) $(DEBUG_FLAGS) $(MEMSAN_FLAGS) $< -o $@

${OBJ}/$(SUB)/forth_p.d : ${OBJ}/$(SUB)/forth_p.cpp
	$(CXX) -MM $(CXXFLAGS) -I$(INC) $(DEFINES) $(OPTIMIZED_FLAGS) $(PROJECT_INCLUDE) $< -o $@
	echo "include $@" >> $(OBJ)/$(SUB)/depend.mk

${OBJ}/$(SUB)/forth_p.ubsan.o : ${OBJ}/$(SUB)/forth_p.cpp
	$(CXX) -c $(CXXFLAGS) -I$(INC) $(DEFINES) $(PROJECT_INCLUDE) $(DEBUG_FLAGS) $(UBSAN_FLAGS) $< -o $@

