#########################################
#				My Makefile				#
#########################################

GCC = g++

workspace:
	$(GCC) workspace.cpp -o ouput
	./ouput
	rm -rf ouput

delta: func.o delta.o
	@$(GCC) func.o delta.o -o ouput
	@./ouput
	@rm -rf *.o
	@rm -rf ouput

delta.o:
	@$(GCC) -c delta.cpp -I./*

func.o:
	@$(GCC) -c func.cpp -I./*

main:
	@$(GCC) -o ouput main.cpp
	@./ouput
	@rm -rf ouput

struct_vector:
	@$(GCC) -o ouput struct_vector.c
	@./ouput
	@rm -rf ouput

num_print:
	@$(GCC) -o ouput num_print.cpp
	@./ouput
	@rm -rf ouput

test_json: test_json.o cJSON.o
	@$(GCC) test_json.o cJSON.o -o ouput
	@./ouput
	@rm -rf *.o
	@rm -rf ouput

test_json.o:
	@$(GCC) -c test_json.cpp -I./*

cJSON.o:
	@$(GCC) -c ./cJSON/cJSON.c -I./cJSON/*

run:
	@./ouput
	
clean:
	@rm -rf *.o
	@rm -rf ouput
