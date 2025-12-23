CC = g++
CFLAGS = --std=c++20 -Wall -Werror -pedantic -g
LIB = -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
# Your .hpp files ppu.h
DEPS = cpu.h memory.h emulator.h util.h instructions.h disassembler.h ppu.h
# Your compiled .o files ppu.o
OBJECTS = cpu.o memory.o emulator.o util.o ppu.o disassembler.o instructions.o arithmetic_instr.o cb_instr.o con_trans_instr.o load_instr.o
# The name of your program
PROGRAM = DrBoy
# Any arguments to pass to your program when you run 'make run'
ARGS = 

.PHONY: all clean lint

all: $(PROGRAM) $(PROGRAM).a	

# Wildcard recipe to make .o files from corresponding .cpp file
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $<

$(PROGRAM): main.o $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

$(PROGRAM).a : $(OBJECTS)
	ar rcs $(PROGRAM).a $(OBJECTS)

run: $(PROGRAM)
	./$(PROGRAM) $(ARGS)

clean:
	rm *.o $(PROGRAM) *.a

lint:
	cpplint *.cpp *.hpp