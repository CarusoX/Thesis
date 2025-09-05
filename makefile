# Define compiler
CXX := g++
FC := gfortran

# Compiler flags
CXXFLAGS := -std=c++17 -Wall -Wextra -O3 -MMD
FCFLAGS := -O2

# Source files
SRC := $(wildcard *.cpp)
SRC += $(wildcard */*.cpp)  # Include subdirectories if needed

# Object files
OBJDIR := obj
OBJ := $(addprefix $(OBJDIR)/, $(notdir $(SRC:.cpp=.o)))

# Executable directory
EXECDIR := exec

# Executable names
FINDER := $(EXECDIR)/drop_finder
SORTER := $(EXECDIR)/drop_sorter
CHART := $(EXECDIR)/drop_chart
CARGA_VELOCIDAD := $(EXECDIR)/carga_velocidad
# Include directories
INCLUDES := -I.

# Libraries (add any required libraries)
LIBS := 

all: $(OBJDIR) ${EXECDIR} $(FINDER) $(SORTER) $(CHART) $(CARGA_VELOCIDAD)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(EXECDIR):
	mkdir -p $(EXECDIR)

$(CARGA_VELOCIDAD): carga_velocidad.f90 | $(EXECDIR)
	$(FC) $(FCFLAGS) -o $@ $<

$(SORTER): $(filter-out $(OBJDIR)/drop_finder.o $(OBJDIR)/drop_chart.o, $(OBJ))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

$(CHART): $(filter-out $(OBJDIR)/drop_sorter.o $(OBJDIR)/drop_finder.o, $(OBJ))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

$(FINDER): $(filter-out $(OBJDIR)/drop_sorter.o $(OBJDIR)/drop_chart.o, $(OBJ))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean

clean:
	rm -rf $(OBJDIR) $(EXECDIR)
