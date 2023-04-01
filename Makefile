INTERPRETER_BIN := lisppp

BUILDDIR := ./build

SOURCES := $(shell find . -name "*.cpp")
OBJECTS := $(patsubst %, $(BUILDDIR)/%.o, $(SOURCES))

LD = g++
CXX = g++
CXXFLAGS = -Wall -Wextra -c -g

.PHONY:
all: $(INTERPRETER_BIN)

$(INTERPRETER_BIN): $(OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $(INTERPRETER_BIN)

$(BUILDDIR)/%.cpp.o: %.cpp | $(BUILDDIR)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILDDIR):
	mkdir $@

.PHONY:
clean:
ifneq ("$(wildcard $(INTERPRETER_BIN))", "")
	rm $(INTERPRETER_BIN)
endif
ifneq ("$(wildcard $(BUILDDIR))", "")
	rm -r $(BUILDDIR)
endif
