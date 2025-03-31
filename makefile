# Define compiler
CXX := g++

# Compiler flags
CXXFLAGS := -std=c++17 -Wall -Wextra -O3 -MMD

# Source files
SRC := $(wildcard *.cpp)
SRC += $(wildcard */*.cpp)  # Include subdirectories if needed

# Object files
OBJDIR := obj
OBJ := $(addprefix $(OBJDIR)/, $(notdir $(SRC:.cpp=.o)))

# Executable directory
EXECDIR := exec

# Executable names
EXTRACTOR := $(EXECDIR)/drop_extractor
SORTER := $(EXECDIR)/drop_sorter
CHART := $(EXECDIR)/drop_chart

# Include directories
INCLUDES := -I.

# Libraries (add any required libraries)
LIBS := 

all: $(OBJDIR) ${EXECDIR} $(EXTRACTOR) $(SORTER) $(CHART)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(EXECDIR):
	mkdir -p $(EXECDIR)

$(EXTRACTOR): $(filter-out $(OBJDIR)/drop_sorter.o $(OBJDIR)/drop_chart.o, $(OBJ))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

$(SORTER): $(filter-out $(OBJDIR)/drop_extractor.o $(OBJDIR)/drop_chart.o, $(OBJ))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

$(CHART): $(filter-out $(OBJDIR)/drop_sorter.o $(OBJDIR)/drop_extractor.o, $(OBJ))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean

clean:
	rm -rf $(OBJDIR) $(EXECDIR)

buscar_gotas:
	./${EXTRACTOR} $(archivo)

ordenar_gotas:
	./${SORTER}

graficar:
	./${CHART}
