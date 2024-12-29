
CC=g++ -Wall -g -O2 -std=c++20

OUT=compiler
OBJ=\
    src/main.o


$(OUT): $(OBJ)
	$(CC) $^ -o $@
%.o: %.cc
	$(CC) -c $< -o $@

.PHONY: clean sysheader

clean:
	rm -rf *.o gcm.cache
