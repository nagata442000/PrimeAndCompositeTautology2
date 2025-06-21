# Makefile for Prime and Composite C++ Programs

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2

# Source files
CORE_SOURCES = core.cpp
CORE_OBJECTS = $(CORE_SOURCES:.cpp=.o)

# Executable programs
PROGRAMS = is_prime prime_factoring_cnf add_cnf prime_and_composite_tautology

# Default target
all: $(PROGRAMS)

# Compile core library
core.o: core.cpp core.hpp
	$(CXX) $(CXXFLAGS) -c core.cpp -o core.o

# Compile is_prime program
is_prime: is_prime.cpp core.o
	$(CXX) $(CXXFLAGS) is_prime.cpp core.o -o is_prime

# Compile prime_factoring_cnf program
prime_factoring_cnf: prime_factoring_cnf.cpp core.o
	$(CXX) $(CXXFLAGS) prime_factoring_cnf.cpp core.o -o prime_factoring_cnf

# Compile add_cnf program
add_cnf: add_cnf.cpp core.o
	$(CXX) $(CXXFLAGS) add_cnf.cpp core.o -o add_cnf

# Compile prime_and_composite_tautology program
prime_and_composite_tautology: prime_and_composite_tautology.cpp core.o
	$(CXX) $(CXXFLAGS) prime_and_composite_tautology.cpp core.o -o prime_and_composite_tautology

# Clean target
clean:
	rm -f $(PROGRAMS) $(CORE_OBJECTS)

# Clean CNF files only
clean-cnf:
	rm -f *.cnf

# Test target - run some basic tests
test: is_prime prime_factoring_cnf add_cnf
	@echo "Testing is_prime with 17..."
	./is_prime 17
	@echo "Testing is_prime with 15..."
	./is_prime 15
	@echo "Testing prime_factoring_cnf with 57..."
	./prime_factoring_cnf 57
	@echo "Testing add_cnf with 3 and 5..."
	./add_cnf 3 5

# Help target
help:
	@echo "Available targets:"
	@echo "  all                    - Build all programs"
	@echo "  is_prime               - Build is_prime program"
	@echo "  prime_factoring_cnf    - Build prime_factoring_cnf program"
	@echo "  add_cnf                - Build add_cnf program"
	@echo "  prime_and_composite_tautology - Build prime_and_composite_tautology program"
	@echo "  clean                  - Remove all executables and object files"
	@echo "  clean-cnf              - Remove only CNF files"
	@echo "  test                   - Run basic tests"
	@echo "  help                   - Show this help message"

# Phony targets
.PHONY: all clean clean-cnf test help

# Dependencies
is_prime: core.hpp
prime_factoring_cnf: core.hpp
add_cnf: core.hpp
prime_and_composite_tautology: core.hpp 