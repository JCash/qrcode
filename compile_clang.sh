mkdir -p build

clang -c src/stb_wrapper.c -o build/stb_wrapper.o

#clang -o ./build/qrencode -g -O3 -m64 -Wall -Weverything -pedantic -Werror -Isrc build/stb_wrapper.o src/encode.c
clang -o ./build/qrencode -g -O3 -m64 -Isrc build/stb_wrapper.o src/encode.c
