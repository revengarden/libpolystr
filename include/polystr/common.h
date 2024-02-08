#ifndef PSTR_COMMON_H
#define PSTR_COMMON_H

#ifdef HAVE_STDIO_H
#include <stdio.h>
#else
#error "stdio.h is required and not supported by your platform."
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
#error "stdlib.h is required and not supported by your platform."
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#else
#error "stddef.h is required and not supported by your platform."
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;
typedef unsigned char uint8_t;
typedef unsgined short uint16_t;
typedef unsgined int uint32_t;
typedef unsgined long long uint64_t;
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#error "string.h is required and not supported by your platform."
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
#error "errno.h is required and not supported by your platform."
#endif

#endif
