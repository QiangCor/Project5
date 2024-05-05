#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "cache.h"
#include "print_helpers.h"

cache_t *make_cache(int capacity, int block_size, int assoc, enum protocol_t protocol, bool lru_on_invalidate_f){
  cache_t *cache = malloc(sizeof(cache_t));
  cache->stats = make_cache_stats();
  
  cache->capacity = capacity;      // in Bytes
  cache->block_size = block_size;  // in Bytes
  cache->assoc = assoc;            // 1, 2, 3... etc.

  // FIX THIS CODE!
  // first, correctly set these 5 variables. THEY ARE ALL WRONG
  // note: you may find math.h's log2 function useful
  #include <math.h>


  cache->n_cache_line = cache->capacity / cache->block_size;
  cache->n_set = cache->capacity/(cache->block_size * cache->assoc);
  cache->n_offset_bit = log2(cache->block_size);
  cache->n_index_bit = log2(cache->capacity/(cache->block_size * cache->assoc));
  cache->n_tag_bit = 32 - cache->n_offset_bit - cache->n_index_bit;

  // next create the cache lines and the array of LRU bits
  // - malloc an array with n_rows
  // - for each element in the array, malloc another array with n_col
  // FIX THIS CODE!

  cache->lines = malloc(sizeof(cache_line_t*) * cache->n_set);
  cache->lru_way = malloc(sizeof(int) * cache->n_set);

  
  // initializes cache tags to 0, dirty bits to false,
  // state to INVALID, and LRU bits to 0
  // FIX THIS CODE!
  

  for (int i = 0; i < cache->n_set; i++) {
    cache->lines[i] = malloc(sizeof(cache_line_t) * cache->assoc);
    cache->lru_way[i] = 0;

    for (int j = 0; j < cache->assoc; j++) {
      cache->lines[i][j].tag = 0;
      cache->lines[i][j].dirty_f = false;
      cache->lines[i][j].state = INVALID;
    }
  }
  
  

  cache->protocol = protocol;
  cache->lru_on_invalidate_f = lru_on_invalidate_f;
  
  return cache;
  
}




/* Given a configured cache, returns the tag portion of the given address.
 *
 * Example: a cache with 4 bits each in tag, index, offset
 * in binary -- get_cache_tag(0b111101010001) returns 0b1111
 * in decimal -- get_cache_tag(3921) returns 15 
 */
unsigned long get_cache_tag(cache_t *cache, unsigned long addr) {
  // FIX THIS CODE!
  unsigned long mask = (1UL << cache->n_tag_bit) - 1; ;
  unsigned long tag = (addr >> (cache->n_index_bit + cache->n_offset_bit)) & mask;
  return tag;
  
}

/* Given a configured cache, returns the index portion of the given address.
 *
 * Example: a cache with 4 bits each in tag, index, offset
 * in binary -- get_cache_index(0b111101010001) returns 0b0101
 * in decimal -- get_cache_index(3921) returns 5
 */
unsigned long get_cache_index(cache_t *cache, unsigned long addr) {
  // FIX THIS CODE!
  unsigned long mask = (1UL << cache->n_index_bit) - 1;
  unsigned long index = (addr << cache->n_tag_bit);
  index = index >> (cache->n_tag_bit+cache->n_offset_bit) & mask;
  return index;
  
}

/* Given a configured cache, returns the given address with the offset bits zeroed out.
 *
 * Example: a cache with 4 bits each in tag, index, offset
 * in binary -- get_cache_block_addr(0b111101010001) returns 0b111101010000
 * in decimal -- get_cache_block_addr(3921) returns 3920
 */
unsigned long get_cache_block_addr(cache_t *cache, unsigned long addr) {
  // FIX THIS CODE!
  unsigned long cache_block_addr = (addr >> cache->n_tag_bit);
  cache_block_addr = cache_block_addr << cache->n_tag_bit;

  return cache_block_addr;

}


/* this method takes a cache, an address, and an action
 * it proceses the cache access. functionality in no particular order: 
 *   - look up the address in the cache, determine if hit or miss
 *   - update the LRU_way, cacheTags, state, dirty flags if necessary
 *   - update the cache statistics (call update_stats)
 * return true if there was a hit, false if there was a miss
 * Use the "get" helper functions above. They make your life easier.
 */
/*
bool access_cache(cache_t *cache, unsigned long addr, enum action_t action) {
  // FIX THIS CODE!
 
  return true;  // cache hit should return true
}
*/

bool access_cache(cache_t *cache, unsigned long addr, enum action_t action) {
  unsigned long tag = get_cache_tag(cache, addr);
  unsigned long index = get_cache_index(cache, addr);
 

  // Look up the address in the cache
  for (int i = 0; i < cache->assoc; i++) {
    if (cache->lines[index][i].tag == tag && cache->lines[index][i].state != INVALID) {
      // Cache hit
      if (action == LOAD) {
        // Update cache statistics for read access
        update_stats(cache->stats, true, false, false, LOAD);
      } else if (action == STORE) {
        // Update cache statistics for write access
        update_stats(cache->stats, true, false, false, STORE);
        cache->lines[index][i].dirty_f = true; // Set dirty flag
      }
      // Update LRU_way
      cache->lru_way[index] = (i+1) % cache->assoc;
      return true;
    }
  }
  /*
  // Cache miss
  if (action == LOAD) {
    // Update cache statistics for read access
    update_stats(cache->stats, false, false, true, LOAD);
  } else if (action == STORE) {
    // Update cache statistics for write access
    update_stats(cache->stats, false, false, true, STORE);
  }

  // Find the LRU line
  
  int lru_line = cache->lru_way[index];
  cache->lru_way[index]= (lru_line + 1) % cache->assoc;

  // Update cache tags, state, and dirty flags
  cache->lines[index][lru_line].tag = tag;
  cache->lines[index][lru_line].state = VALID;
  cache->lines[index][lru_line].dirty_f = false;
  */
  
  int lru_line = cache->lru_way[index];
  cache->lru_way[index]= (lru_line + 1) % cache->assoc;
  bool wb = cache->lines[index][lru_line].dirty_f && (cache->lines[index][lru_line].state = VALID); 

  // Update cache tags, state, and dirty flags
  cache->lines[index][lru_line].tag = tag;
  cache->lines[index][lru_line].state = VALID;
  cache->lines[index][lru_line].dirty_f = (action == STORE);

  // Cache miss
  if (action == LOAD) {
    // Update cache statistics for read access
    update_stats(cache->stats, false, wb, false, LOAD);
  } else if (action == STORE) { 
    // Update cache statistics for write access
    update_stats(cache->stats, false, wb, false, STORE);
  }

  // Update LRU_way
  // cache->lru_way[index] = lru_line;

  return false;  // cache miss should return false
}

