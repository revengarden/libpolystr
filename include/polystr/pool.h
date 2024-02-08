#ifndef PSTR_POOL_H
#define PSTR_POOL_H

#include "common.h"

typedef struct string_memory_pool_s {
  char *data;
  char *last;
  uint32_t capacity;
  uint32_t count;
} string_memory_pool_t;

int8_t string_memory_pool_new(string_memory_pool_t *, uint32_t);
int8_t string_memory_pool_free(string_memory_pool_t *);
int8_t string_memory_pool_push(string_memory_pool_t *, char **, size_t,
                               uint32_t *);
int8_t string_memory_pool_get(string_memory_pool_t *, uint32_t, char **);
int8_t string_memory_pool_swap_to_file(string_memory_pool_t *pool,
                                       const char *file_name);
int8_t string_memory_pool_swap_to_pool(string_memory_pool_t *pool,
                                       size_t capacity, const char *file_name);

#endif
