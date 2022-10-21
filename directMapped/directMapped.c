#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// Rutgers University 2021 Spring CS 211 Computer Architecture
// Assignment based on CS:APP Cache Lab

// 64-bit memory address
typedef unsigned long long int mem_addr_t;

// cache properties
unsigned char s = 4; // set index bits
unsigned short S = 1<<4; // set count

unsigned char b = 4; // block offset bits
// unsigned short B = 1<<4; // block size in bytes

// direct mapped cache set/line
typedef struct cache_set_line {
    bool valid;
    mem_addr_t tag;
} cache_set_line_t;

typedef cache_set_line_t* cache_t;

// accessData - Access data at memory address addr.
void accessData (
    mem_addr_t addr,
    cache_t cache,
    unsigned int* hit_count, // If it is already in cache, increase hit_count
    unsigned int* miss_count, // If it is not in cache, bring it in cache, increase miss_count
    unsigned int* eviction_count // Also increase eviction_count if a line is evicted
) {

    // Cache indices for this address
    // mem_addr_t index = (addr >> (s+b)) % s; // already shifted, needs to mask
    mem_addr_t index = addr >> (s) & ((1<<s)-1); // from toHex modified
    mem_addr_t tag = addr >> (s+b);

    //printf("index: %lld, tag: %lld, currentTag: %lld ", index, tag, cache[index].tag);

    if (cache[index].tag == tag && cache[index].valid) {
        (*hit_count)++;
        //printf("HIT\n");
        return;
    }
    if (!cache[index].valid) {
        (*miss_count)++;
        cache[index].tag = tag;
        cache[index].valid = true;
        //printf("MISS\n");
    }
    if (cache[index].tag != tag && cache[index].valid) {
        (*eviction_count)++;
        (*miss_count)++;
        cache[index].tag = tag;
        //printf("EVICT\n");
    }

    cache[index].valid = true;

    // Cache hit
    /* ... */

    // Otherwise, cache miss
    /* ... */

    // If cache set line already in use as indicated by the valid variable, then evict the existing cache set line
    /* ... */
}

int main(int argc, char* argv[]) {

    // path to memory trace
    if ( argc!= 2 ) {
        printf( "Usage: ./directMapped <mem_trace_file>" );
    }
    char* mem_trace_file = argv[1];
    FILE *fp = fopen(mem_trace_file, "r");
    if (!fp) {
        fprintf(stderr, "Error opening file '%s'\n", mem_trace_file);
        exit( EXIT_FAILURE );
    }

    // Allocate memory, write 0's for valid and tag and LRU
    cache_t cache = (cache_set_line_t*) calloc( S, sizeof(cache_set_line_t) );
    for (int i = 0; i < S; i++) {
        cache[i].valid = 0;
        cache[i].tag = 0;
    }

    // cache simulation statistics
    unsigned int miss_count = 0;
    unsigned int hit_count = 0;
    unsigned int eviction_count = 0;

    size_t line_buf_size = 256;
    char line_buf[line_buf_size];

    while ( fgets(line_buf, line_buf_size, fp) != NULL ) {

        // replay the given trace file against the cache
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

    fclose(fp);
    printf("hits:%d misses:%d evictions:%d\n", hit_count, miss_count, eviction_count);

    exit( EXIT_SUCCESS );
    /* ... */
}
