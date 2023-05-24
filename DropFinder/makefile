# Define compiler
CXX := g++

# Compiler flags
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Source files
SRC := $(wildcard *.cpp)

# Object files
OBJDIR := obj
OBJ := $(addprefix $(OBJDIR)/, $(notdir $(SRC:.cpp=.o)))

# Executable name
EXEC := drop_finder

all: $(OBJDIR) $(EXEC)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -rf $(OBJDIR) $(EXEC)