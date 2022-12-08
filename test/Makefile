#########################################
#				My Makefile				#
#########################################

GCC = g++

removeDuplicates:
	@g++ removeDuplicates.cpp -o output
	@./output
	@rm -rf output

mergeTwoLists:
	@g++ mergeTwoLists.cpp -o output
	@./output
	@rm -rf output

validParentheses:
	@g++ validParentheses.cpp -o output
	@./output
	@rm -rf output

longestCommonPrefix:
	@g++ longestCommonPrefix.cpp -o output
	@./output
	@rm -rf output

AddTwoNumbers:
	@$(GCC) -o output ./AddTwoNumbers.cpp
	@./output
	@rm -rf output

main:
	@g++ main.cpp -o output
	@./output
	@rm -rf output

romanToInt:
	@g++ romanToInt.cpp -o output
	@./output
	@rm -rf output

isPalindrome:
	@g++ isPalindrome.cpp -o output
	@./output
	@rm -rf output

twoSum:
	@g++ twoSum.cpp -o output
	@./output
	@rm -rf output

lengthOfLongestSubstring:
	@g++ lengthOfLongestSubstring.cpp -o output
	@./output
	@rm -rf output

test_namespace:
	$(GCC) test_namespace.cpp -o output
	./output
	rm -rf output

exception:
	$(GCC) exception.cpp -o output
	./output
	rm -rf output

workspace:
	$(GCC) workspace.cpp -o output
	./output
	rm -rf output

struct_vector:
	@$(GCC) -o output struct_vector.c
	@./output
	@rm -rf output

template:
	@$(GCC) -o output template.cpp
	@./output
	@rm -rf output

numPrint:
	@$(GCC) -o output num_print.cpp
	@./output
	@rm -rf output

testJson: testJson.o cJSON.o
	@$(GCC) testJson.o cJSON.o -o output
	@./output
	@rm -rf *.o
	@rm -rf output

testJson.o:
	@$(GCC) -c testJson.cpp -I./*

cJSON.o:
	@$(GCC) -c ../library/cjson/cJSON.c -I./cJSON/*

run:
	@./output
	
clean:
	@rm -rf *.o
	@rm -rf output
