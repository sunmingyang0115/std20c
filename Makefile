
CXX=g++
CXXFLAGS=-Wall -g -std=c++17 -O2
CPPFLAGS=-Iinclude

OUT=std20c
OBJ=\
    build/main.o \
	build/error_message.o \
	build/scan/tokenize.o \
	build/debug.o \
	build/parse/parser.o \
	build/parse/generate_chart.o \
	build/parse/generate_tree.o \
	build/parse/generate_error.o \
	build/analysis/semantics.o \
	build/analysis/semantics_error.o \
	build/analysis/scope.o \
	build/codegen/lower.o
	
$(OUT): $(OBJ)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $^ -o $@
build/%.o: src/%.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: clean sysheader

clean:
	rm -rf *.o gcm.cache build $(OUT)
