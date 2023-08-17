
# Cache Friendly Programming

A set of programs used to model and simulate Cache behavior, followed by a set of programs used to program with the cache in mind.

These projects were created for my Computer Architecture course (CS 214) and include some provided code.

To differentiate between provided code and my personal code, please check the difference between the \*_provided.c file and the \*.c file for each program.

## Authors

- [@Leo Lawrence](https://www.github.com/octokatherine), primary contributor
- [Yipeng Huang](https://yipenghuang.com/), course instructor


## Cache Simulations

### directMapped
- Simulates a directMapped cache (meaning, each address maps to a single, specific location in cache memory).
- Used as part of the logic for setAssociative, since setAssociative combines directMapped caches and fullyAssociative caches.

### fullyAssociative
- Simulates a fullyAssociative cache (meaning, data can be stored in any location of memory).
- Used as part of the logic for setAssociative, since setAssociative combines directMapped caches and fullyAssociative caches.

### setAssociative 
- Simulates a setAssociative cache (combines elements of a directMapped cache and a fullyAssociative cache to hash addresses into memory).
- Uses Least Recently Used (LRU) as collision control.

## Cache Programs

### cacheBlocking
- Uses cacheBlocking to efficiently multiply matricies.

### cacheOblivious
- Uses recursion to transpose matricies.
