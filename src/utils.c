#include <string.h>
#include "utils.h"
#include "macros.h"
#include "config.h"

void get_relative_path(const char* path, char* dir)
{
  if (strlen(path) > MAX_PATH_LEN) { MSG_ERROR("path too long;"); }
  const char* last_slash = strrchr(path, '/');
  if (last_slash != NULL)
  {
    size_t len = strlen(path) - strlen(last_slash);
    strncpy(dir, path, len);
    dir[len] = '/';
    dir[len + 1] = '\0';
  }
  else { strcpy(dir, path); }
}
