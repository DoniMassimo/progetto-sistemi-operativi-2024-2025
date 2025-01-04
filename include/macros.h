#ifndef MACROS_H
#define MACROS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FUNC_PERROR()                                                                       \
  fprintf(stderr, "\nError in file: %s, function: %s, error msg: %s\n", __FILE__, __func__, \
          strerror(errno));                                                                 \
  exit(EXIT_FAILURE);

#define FUNC_MSG_PERROR(msg)    \
  fprintf(stderr, "%s: ", msg); \
  perror(__func__);             \
  exit(EXIT_FAILURE);

#define FUNC_MSG_ERROR(msg)                   \
  fprintf(stderr, "%s: %s\n", __func__, msg); \
  exit(EXIT_FAILURE);

#define MSG_ERROR(msg)          \
  fprintf(stderr, "%s\n", msg); \
  exit(EXIT_FAILURE);

#endif
