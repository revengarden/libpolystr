#include <polystr.h>

#include <stdint.h>
#include <stdio.h>

/**
 * Testing string pool.
 */
int main() {
  uint8_t should_fail = 0;
  string_memory_pool_t pool;
  string_memory_pool_t *ppool = &pool;

  // inital step allocating 0 size pool should fail.
  if (string_memory_pool_new(ppool, 0) != -1) {
    fprintf(stderr, "allocate invalid pool size, size is less than 3.\n");
    return -1;
  }

  pool.data = NULL;
  pool.last = NULL;
  pool.count = 0;
  pool.capacity = 0;

  // TODO: write allocation for more then free memory available.

  // allocate 128 byte of memory pool, which is actually 127 byte free.
  if (string_memory_pool_new(ppool, 128) != 0) {
    fprintf(stderr, "fail in allocation.\n");
    return -1;
  } else {
    {
      int i = 0;
      for (i = 0; i < 128; i++) {
        if (pool.data[i] != 0) {
          should_fail = 1;
          break;
        }
      }

      if (should_fail) {
        fprintf(stderr, "allocated memory should be set to 0.\n");
        return -1;
      }
    }
  }

  // check allocation capacity
  if (pool.capacity != 127) {
    fprintf(stderr, "invalid amount of allocation capacity.\n");
    return -1;
  }

  // check allocation free
  if (string_memory_pool_free(ppool) != 0) {
    fprintf(stderr, "freeing pool failed.\n");
    return -1;
  }

  // fail setting null after freeing.
  if (pool.data != NULL) {
    fprintf(stderr, "fail setting null after freeing.\n");
    return -1;
  }

  // double free should fail.
  if (string_memory_pool_free(ppool) != -1) {
    fprintf(stderr, "double free should fail.\n");
    return -1;
  }

  if (string_memory_pool_new(ppool, 6) != 0) {
    fprintf(stderr, "unable to allocate.\n");
    return -1;
  }

  char *str = "Hello";
  uint32_t offset = 0;
  if (string_memory_pool_push(ppool, str, &str, 0, &offset) != -1) {
    fprintf(stderr, "null terminated string should not fit.\n");
    return -1;
  }

  char *str_ref = str;
  str = "Hell";
  if (string_memory_pool_push(ppool, str, &str, 0, &offset) != 0) {
    fprintf(stderr, "null terminated string should fit.\n");
    return -1;
  } else {
    // check offset return
    if (offset != 1) {
      fprintf(stderr, "wrong offset return.\n");
      return -1;
    }

    if (str_ref == str) {
      fprintf(stderr, "returned string reference should differ.\n");
      return -1;
    }

    if (pool.capacity != 0) {
      fprintf(stderr, "capacity should be 0 after full string push.\n");
      return -1;
    }

    if (pool.count != 1) {
      fprintf(stderr, "string count in pool should be incremented.\n");
      return -1;
    }

    if (strncmp("Hell", str, 5) != 0) {
      fprintf(stderr, "string is not the same.\n");
      return -1;
    }

    if (string_memory_pool_free(ppool) != 0) {
      fprintf(stderr, "unable to free allocated pool.\n");
      return -1;
    } else {
      if (strncmp("Hell", str, 5) == 0) {
        fprintf(stderr, "string is not deallocated.\n");
        return -1;
      }
    }

    // test string_memory_pool_get
    {
      uint32_t offset = 0;
      char *str = "Hello";
      string_memory_pool_new(ppool, 100);
      string_memory_pool_push(ppool, str, &str, 0, &offset);

      if (pool.capacity != 93) {
        fprintf(stderr, "capacity should be 93 if Hello is pushed first.\n");
        return -1;
      }
      char *str2 = "Hello2";
      string_memory_pool_push(ppool, str2, &str2, 0, &offset);
      if (offset != 7) {
        fprintf(stderr, "invalid offset value.\n");
        return -1;
      }

      if (pool.capacity != (93 - 7)) {
        fprintf(stderr, "pool capacity is does not match after second push.\n");
        return -1;
      }

      char *str_out;
      if (string_memory_pool_get(ppool, offset, &str_out) != 0) {
        fprintf(stderr, "could not get string, get method fail.\n");
        return -1;
      } else {
        if (strncmp("Hello2", str_out, 6) != 0) {
          fprintf(stderr,
                  "invalid string pushed, string at offset %d does not match\n",
                  offset);
          return -1;
        }
      }

      string_memory_pool_free(ppool);
    }

    { // Swap To File
      string_memory_pool_new(ppool, 100);
      if (string_memory_pool_swap_to_file(ppool, "tmp_file") != 1) {
        fprintf(stderr, "empty.\n");
        return -1;
      }

      char *str1 = "Test";
      uint32_t offset = 0;
      string_memory_pool_push(ppool, str1, &str1, 0, &offset);

      if (string_memory_pool_swap_to_file(ppool, "/no_perm_file") >= 0) {
        fprintf(stderr, "file errors not handled.\n");
        return -1;
      }

      if (string_memory_pool_swap_to_file(ppool, "swap1.tmp") != 0) {
        fprintf(stderr, "error writing to file.\n");
        return -1;
      } else {
        string_memory_pool_free(ppool);
        if (string_memory_pool_swap_to_pool(ppool, 50, "swap1.tmp") != 0) {
          fprintf(stderr, "error handling swap file to memory.\n");
          return -1;
        }
        string_memory_pool_get(ppool, offset, &str1);
        if (strncmp("Test", str1, 5) != 0) {
          fprintf(stderr, "string in swap file is not the same.\n");
          return -1;
        }

        if (pool.capacity != 45) {
          fprintf(stderr, "invalid new pool capacity.\n");
          return -1;
        }
        string_memory_pool_free(ppool);

        // String pool of lesser size request should become maximum size of swap
        // file:
        if (string_memory_pool_swap_to_pool(ppool, 4, "swap1.tmp") != 0) {
          fprintf(stderr, "error handling swap file to memory.\n");
          return -1;
        }

        string_memory_pool_get(ppool, offset, &str1);
        if (strncmp("Test", str1, 5) != 0) {
          fprintf(stderr, "string in swap file is not the same.\n");
          return -1;
        }

        if (pool.capacity != 0) {
          fprintf(stderr, "invalid new pool capacity.\n");
          return -1;
        }
        string_memory_pool_free(ppool);
      }

      remove("swap1.tmp");
    }
  }

  return 0;
}
