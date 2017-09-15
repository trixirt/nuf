TOP=../../..
include $(TOP)/build/makefiles/MakeMacros.pre

SUB=scan
SOBJS=${OBJ}/$(SUB)/scanner.o \
	${OBJ}/$(SUB)/uscanner.o \
	${OBJ}/$(SUB)/forth_s.o

TARGET=$(OBJ)/$(SUB)/libscan.a
DEBUG_TARGET=$(OBJ)/$(SUB)/libscan.debug.a
MEMSAN_TARGET=$(OBJ)/$(SUB)/libscan.memsan.a
UBSAN_TARGET=$(OBJ)/$(SUB)/libscan.ubsan.a
SCAN_TARGET=$(OBJ)/$(SUB)/libscan.scan.a
INC+=-I$(OBJ)

all: dirs $(TARGET) $(DEBUG_TARGET) $(MEMSAN_TARGET) $(SCAN_TARGET) $(UBSAN_TARGET)

include $(TOP)/build/makefiles/MakeMacros

${OBJ}/$(SUB)/forth_s.cpp : $(SRC)/$(SUB)/forth.re
	$(RE2C) -W $< > $@

${OBJ}/$(SUB)/forth_s.o : ${OBJ}/$(SUB)/forth_s.cpp
	$(CXX) -c $(CXXFLAGS) -I$(INC) $(DEFINES) $(OPTIMIZED_FLAGS) $(PROJECT_INCLUDE) $< -o $@

${OBJ}/$(SUB)/forth_s.debug.o : ${OBJ}/$(SUB)/forth_s.cpp
	$(CXX) -c $(CXXFLAGS) -I$(INC) $(DEFINES) $(PROJECT_INCLUDE) $(DEBUG_FLAGS) $< -o $@

${OBJ}/$(SUB)/forth_s.memsan.o : ${OBJ}/$(SUB)/forth_s.cpp
	$(CXX) -c $(CXXFLAGS) -I$(INC) $(DEFINES) $(PROJECT_INCLUDE) $(DEBUG_FLAGS) $(MEMSAN_FLAGS) $< -o $@

${OBJ}/$(SUB)/forth_s.ubsan.o : ${OBJ}/$(SUB)/forth_s.cpp
	$(CXX) -c $(CXXFLAGS) -I$(INC) $(DEFINES) $(PROJECT_INCLUDE) $(DEBUG_FLAGS) $(UBSAN_FLAGS) $< -o $@

${OBJ}/$(SUB)/forth_s.scan.o : ${OBJ}/$(SUB)/forth_s.cpp
	$(SCAN)	$(CXX) -c $(CXXFLAGS) -I$(INC) $(DEFINES) $(PROJECT_INCLUDE) $(DEBUG_FLAGS) $(MEMSAN_FLAGS) $< -o $@

${OBJ}/$(SUB)/forth_s.d : ${OBJ}/$(SUB)/forth_s.cpp
	$(CXX) -MM $(CXXFLAGS) -I$(INC) $(DEFINES) $(OPTIMIZED_FLAGS) $(PROJECT_INCLUDE) $< -o $@
	echo "include $@" >> $(OBJ)/$(SUB)/depend.mk
