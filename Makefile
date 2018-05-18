all:
	clang++ -Werror -Ofast -std=c++14 -g -O0 example/*.cpp serializer/*.cpp libs/*.cpp -o build/serializer-test
