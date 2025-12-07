# Compiler and options
CXX := icpx
CXXFLAGS := -fsycl -std=c++20 -Wall -O2
INCLUDES := -Iinclude -I/home/arunc/hdf5-parallel/include
OBJDIR := obj
TARGET := bin/Kobayshi_model

LIBS := -L/home/arunc/hdf5-parallel/lib -lhdf5 -lhdf5_cpp

# CUDA GPU + disable device-aware MPI
ifeq ($(GPU),1)
	CXXFLAGS += -DGPU 
	CXXFLAGS += -fsycl-targets=nvptx64-nvidia-cuda
endif


# source files
SRC := \
    src/main.cpp \
	src/utilities/IO.cpp

# Corresponding object files
OBJ := $(OBJDIR)/main.o \
	   $(OBJDIR)/utilities/IO.o 

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	@mkdir -p DATA
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean_obj:
	rm -rf $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)

rdata:
	rm -rf DATA

crun: clean $(TARGET)
	./$(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean rm data clean_obj