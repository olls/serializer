all:
	clang++ -Werror -Ofast -std=c++14 -g -O0 *.cpp -o build/serializer-test