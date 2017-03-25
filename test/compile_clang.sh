mkdir -p build

#clang -o ./build/testencode -g -O0 -std=c99 -m64 -Wall -Weverything -pedantic -Werror -I../src testencode.c
clang -o ./build/testencode -g -O0 -std=c99 -m64 -I../src testencode.c
