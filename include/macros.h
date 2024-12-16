#ifndef MACROS_H
#define MACROS_H
#include <stdio.h>
#include <stdlib.h>

#define FUNC_PERROR() \
  perror(__func__);   \
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
