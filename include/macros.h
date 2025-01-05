#ifndef MACROS_H
#define MACROS_H

#include "log.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FUNC_PERROR()         \
  log_fatal(strerror(errno)); \
  exit(EXIT_FAILURE);

#define MSG_ERROR(msg) \
  log_fatal(msg);      \
  exit(EXIT_FAILURE);

#endif
