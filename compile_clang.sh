mkdir -p build

#clang -o ./build/qrencode -g -O3 -m64 -Wall -Weverything -pedantic -Werror -Isrc src/encode.c
clang -o ./build/qrencode -g -O3 -m64 -Wall -Isrc src/encode.c
