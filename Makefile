
CC=g++ -Wall -g -O2

OUT=compiler
OBJ=\
    build/main.o \
	build/error_message.o \
	build/scan/tokenize.o \
	build/parse/parser.o \
	build/parse/generate_chart.o \
	build/parse/generate_tree.o \
	build/parse/generate_error.o \
	build/analysis/semantics.o \
	build/analysis/semantics_error.o \
	build/analysis/scope.o
	# build/codegen/code_generation.o
	


$(OUT): $(OBJ)
	$(CC) $^ -o $@
build/%.o: src/%.cc
	@mkdir -p $(@D)
	$(CC) -c $< -o $@

.PHONY: clean sysheader

clean:
	rm -rf *.o gcm.cache build $(OUT)
