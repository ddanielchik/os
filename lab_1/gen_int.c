// gen_int.c

// Сборка: gcc -std=c11 -O0 -Wall -Wextra gen_int.c -o gen_int

// usage: ./gen_int <output-file> <count> [seed]

// Создаёт бинарный файл из count int64_t.



#include <stdio.h>

#include <stdlib.h>

#include <stdint.h>

#include <time.h>



int main(int argc, char **argv){

    if(argc < 3){

        fprintf(stderr, "Usage: %s <out.bin> <count> [seed]\n", argv[0]);

        return 1;

    }

    const char *out = argv[1];

    long long count = atoll(argv[2]);

    unsigned seed = (argc >= 4) ? (unsigned)atoi(argv[3]) : (unsigned)time(NULL);

    srand(seed);



    FILE *f = fopen(out, "wb");

    if(!f){ perror("fopen"); return 2; }



    for(long long i = 0; i < count; ++i){

        int64_t v = ((int64_t)rand() << 32) ^ rand() ^ (int64_t)i;

        if(fwrite(&v, sizeof(v), 1, f) != 1){ perror("fwrite"); fclose(f); return 3; }

    }

    fclose(f);

    return 0;

}

