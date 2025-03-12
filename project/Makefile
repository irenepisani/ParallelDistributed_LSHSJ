# Compilers
CXX	   +=-std=c++20
MPICXX = mpicxx -std=c++20

# Includes 
INCLUDES    = -I ./dependencies 
CXX_INCLUDES = -I ./fastflow

# Other flags 
#CXX_FLAGS    += -Wall # gives warnings from dependencies files
#MPICXX_FLAGS += -Wall # gives warnings from dependencies files

# Optimization flags 
ifdef DEBUG
    OPT_FLAGS += -g
else
    OPT_FLAGS += -O3 -DNDEBUG -ffast-math
endif

# Fastflow-related optimization flags
ifdef MAPPING
    OPT_FLAGS_FF += -DNO_DEFAULT_MAPPING
endif 
ifdef BLOCKING
    OPT_FLAGS_FF +=-DBLOCKING_MODE 
endif 
ifdef BOUNDED
    OPT_FLAGS_FF += -DFF_BOUNDED_BUFFER -DDEFAULT_BUFFER_CAPACITY=1 
endif

# Library 
CXX_LIBS    = -pthread
MPICXX_LIBS = -fopenmp

# Objects
OBJS = dependencies/frechet_distance.o dependencies/frechet_distance2.o dependencies/geometry_basics.o

# Output (build) and source directories
BUILD_DIR = ./build
SRC_DIR = ./src

# Source files 
CXX_SOURCES = $(SRC_DIR)/LSHSJ_ff.cpp
CXX_SOURCES_SEQ = $(SRC_DIR)/LSHSJ_seq.cpp
MPICXX_SOURCES = $(SRC_DIR)/LSHSJ_mpi.cpp  $(SRC_DIR)/LSHSJ_mpi_nb.cpp  $(SRC_DIR)/LSHSJ_mpi_omp.cpp

# Convert source name to executable names
CXX_TARGETS_FF = $(CXX_SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%)
CXX_TARGETS_SEQ = $(CXX_SOURCES_SEQ:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%)
MPICXX_TARGETS = $(MPICXX_SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%)

# Main rule
all: $(CXX_TARGETS_FF) $(CXX_TARGETS_SEQ) $(MPICXX_TARGETS) 

# Create build dir
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to compile with g++ compiler the sequential code
$(CXX_TARGETS_SEQ): $(BUILD_DIR)/%: $(SRC_DIR)/%.cpp $(OBJS) dependencies/frechet_distance.hpp dependencies/geometry_basics.hpp | $(BUILD_DIR)
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(CXX_INCLUDES) $(OPT_FLAGS) -o $@ $< $(OBJS) $(LDFLAGS) 

# Rule to compile with g++ compiler the FF code
$(CXX_TARGETS_FF): $(BUILD_DIR)/%: $(SRC_DIR)/%.cpp $(OBJS) dependencies/frechet_distance.hpp dependencies/geometry_basics.hpp | $(BUILD_DIR)
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(CXX_INCLUDES) $(OPT_FLAGS) $(OPT_FLAGS_FF) -o $@ $< $(OBJS) $(LDFLAGS) $(CXX_LIBS)

# Rule to compile with mpicxx compiler the MPI code
$(MPICXX_TARGETS): $(BUILD_DIR)/%: $(SRC_DIR)/%.cpp $(OBJS) dependencies/frechet_distance.hpp dependencies/geometry_basics.hpp | $(BUILD_DIR)
	$(MPICXX) $(MPICXX_FLAGS) $(INCLUDES) $(MPICXX_INCLUDES)  $(OPT_FLAGS) -o $@ $< $(OBJS) $(LDFLAGS) $(MPICXX_LIBS)

# Clean executables
clean:
	rm -rf $(BUILD_DIR)
cleanall : clean
	rm -f *.o *~ dependencies/*.o dependencies/*~

.PHONY: all clean cleanall
.SUFFIXES: .cpp 