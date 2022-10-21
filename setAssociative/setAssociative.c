#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// Rutgers University 2021 Spring CS 211 Computer Architecture
// Assignment based on CS:APP Cache Lab

// 64-bit memory address
typedef unsigned long long int mem_addr_t;

// cache properties
unsigned char s = 2; // set index bits
unsigned short S = 1<<2; // set count

unsigned char E = 4; // way associative cache; lines per set

unsigned char b = 4; // block offset bits
// unsigned short B = 1<<4; // block size in bytes

// fully associative cache line
typedef struct cache_line {
    bool valid;
    mem_addr_t tag;
    size_t recent_use; // For keeping track of recent use of cache line
} cache_line_t;

typedef cache_line_t* cache_set_t;
typedef cache_set_t* cache_t;

// For keeping track of recent use of cache line
size_t program_counter = 0;

// accessData - Access data at memory address addr.
void accessData (
    mem_addr_t addr,
    cache_t cache,
    unsigned int* hit_count, // If it is already in cache, increase hit_count
    unsigned int* miss_count, // If it is not in cache, bring it in cache, increase miss_count
    unsigned int* eviction_count // Also increase eviction_count if a line is evicted
) {

    // Cache indices for this address
    mem_addr_t tag = addr >> (s+b);
    
    // Find the correct cache set
    mem_addr_t index = (addr>>b) & 3;

    // Cache hit
    for (int i = 0; i < E; i++) {
        if (cache[index][i].valid && cache[index][i].tag == tag) {
            (*hit_count)++;
            cache[index][i].recent_use = program_counter;
            return;
        }
    }

    // Otherwise, record a cache miss
    (*miss_count)++;

    // Within the correct cache set, find the least recently used line; that is where missed cache block will be placed
    mem_addr_t LSU = 0;
    int min = 2147483647;
    for (mem_addr_t j = 0; j < E; j++) {
        if (cache[index][j].recent_use < min) {
            min = cache[index][j].recent_use;
            LSU = j;
        }
    }
    // place missed cache block
    cache[index][LSU].tag = tag;
    cache[index][LSU].recent_use = program_counter;

    // If cache set line already in use, then record an eviction
    if (cache[index][LSU].valid) {
        (*eviction_count)++;
    }
    else {
        cache[index][LSU].valid = true;
    }

    /* ... */
}

int main(int argc, char* argv[]) {

    // path to memory trace
    if ( argc!= 2 ) {
        printf( "Usage: ./fullyAssociative <mem_trace_file>\n" );
        exit( EXIT_FAILURE );
    }
    char* mem_trace_file = argv[1];
    FILE *fp = fopen(mem_trace_file, "r");
    if (!fp) {
        fprintf(stderr, "Error opening file '%s'\n", mem_trace_file);
        exit( EXIT_FAILURE );
    }

    // Allocate memory, write 0's for valid and tag and LRU
    cache_t cache = calloc( S, sizeof(cache_set_t) );
    for (size_t set_index=0; set_index<S; set_index++) {
        cache[set_index] = calloc( E, sizeof(cache_line_t) );
        for (int i = 0; i < E; i++) {
            cache[set_index][i].valid = 0;
            cache[set_index][i].tag = 0;
            cache[set_index][i].recent_use = 0;
        }
    }



    // cache simulation statistics
    unsigned int miss_count = 0;
    unsigned int hit_count = 0;
    unsigned int eviction_count = 0;

    // Loop through until we are done with the file.
    size_t line_buf_size = 256;
    char line_buf[line_buf_size];
    while ( fgets(line_buf, line_buf_size, fp) != NULL ) {

        program_counter++;

        if ( line_buf[1]=='L' || line_buf[1]=='S' || line_buf[1]=='M' ) {
            char access_type = '\0';
            mem_addr_t addr = 0;
            unsigned int len = 0;
            sscanf ( line_buf, " %c %llx,%u", &access_type, &addr, &len );

            if ( access_type=='L' || access_type=='S' || access_type=='M') {
                accessData(addr, cache, &hit_count, &miss_count, &eviction_count);
            }

            // If the instruction is M indicating L followed by S then access again
            if(access_type=='M')
                accessData(addr, cache, &hit_count, &miss_count, &eviction_count);
        }

    }

    for (size_t set_index=0; set_index<S; set_index++) {
        free(cache[set_index]);
        cache[set_index] = NULL;
    }

    free(cache);
    cache = NULL;
    fclose(fp);

    /* Output the hit and miss statistics for the autograder */
    printf("hits:%d misses:%d evictions:%d\n", hit_count, miss_count, eviction_count);

    exit( EXIT_SUCCESS );
}
