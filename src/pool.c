#include <polystr/pool.h>

int8_t string_memory_pool_new(string_memory_pool_t *pool, uint32_t capacity) {
  // Minimum amount of capacity for memory pool is 3 bytes.
  if (capacity < 3) {
    errno = EINVAL;
    perror("Invalid capacity amount");
    pool->data = NULL;
    return (-1);
  }

  pool->capacity = capacity;
  pool->data = malloc(sizeof(char) * pool->capacity);

  // Check if memory could be allocated.
  if (pool->data == NULL) {
    pool->capacity = 0;
    perror("Unable to allocate new memory");
    return (-2);
  }

  memset(pool->data, 0, pool->capacity);
  pool->last = pool->data + 1;
  pool->capacity -= 1;
  pool->count = 0;
  return (0);
}

int8_t string_memory_pool_free(string_memory_pool_t *pool) {
  if (pool->data == NULL) {
    errno = EINVAL;
    perror("Invalid pool free request");
    return (-1);
  }

  free(pool->data);
  pool->data = NULL;
  pool->last = NULL;
  pool->count = 0;
  pool->capacity = 0;

  return (0);
}

int8_t string_memory_pool_push(string_memory_pool_t *pool, char **string,
                               size_t string_len, uint32_t *offset) {
  *offset = 0;
  // if string_len is 0 find it ourselves.
  if (string_len == 0) {
    string_len = strlen(*string);
  }

  // can't fit string into pool
  if ((signed)(pool->capacity - (string_len + 1)) < 0) {
    return (-1);
  }

  // no need to null terminate since poll is already filled with null.
  strncpy(pool->last, *string, string_len);
  char *string_ptr = pool->last;

  pool->last = pool->last + string_len + 1;
  pool->capacity -= (string_len + 1);
  pool->count += 1;

  // set string to the new pointer
  *string = string_ptr;
  *offset = string_ptr - pool->data;

  return (0);
}

int8_t string_memory_pool_get(string_memory_pool_t *pool, uint32_t offset,
                              char **string_out) {
  if (offset == 0 || pool->data == NULL) {
    return (-1);
  }
  *string_out = pool->data + offset;
  return (0);
}

int8_t string_memory_pool_swap_to_file(string_memory_pool_t *pool,
                                       const char *file_name) {
  if (pool->count == 0) {
    // No error but no file write.
    return 1;
  }

  FILE *file = fopen(file_name, "wb");
  if (file == NULL) {
    return -errno;
  }
  fputc('S', file);
  fputc('S', file);
  fputc('M', file);
  fputc('P', file);

  size_t lastlen = strlen(pool->last);

  int32_t size = (pool->last + lastlen - pool->data - 1);
  // write size of strings
  fwrite((const char *)&size, sizeof(size), 1, file);
  fwrite((const char *)&pool->count, sizeof(pool->count), 1, file);
  fwrite(pool->data, size, 1, file);
  fclose(file);

  return 0;
}

int8_t string_memory_pool_swap_to_pool(string_memory_pool_t *pool,
                                       size_t capacity, const char *file_name) {
  FILE *file = fopen(file_name, "rb");
  if (file == NULL) {
    return -errno;
  }

  if (fgetc(file) != 'S') {
    return -126;
  }
  if (fgetc(file) != 'S') {
    return -126;
  }
  if (fgetc(file) != 'M') {
    return -126;
  }
  if (fgetc(file) != 'P') {
    return -126;
  }

  int32_t size = 0;
  int32_t count = 0;
  fread(&size, sizeof(size), 1, file);
  fread(&count, sizeof(count), 1, file);
  if (capacity < size) {
    capacity = size;
  }
  string_memory_pool_new(pool, capacity);
  pool->capacity -= size - 1;
  fread(pool->data, size, 1, file);
  pool->last = pool->data + size;
  fclose(file);
  return 0;
}
