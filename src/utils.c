#include <string.h>
#include "utils.h"
#include "macros.h"
#include "config.h"

void utils_get_relative_path(const char* path, char* dir)
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

void utils_join_str(char* joined_str, char* str1, char* str2)
{
  strcpy(joined_str, str1);
  strcat(joined_str, str2);
}

void utils_assign_count_array(int* count_ar, int ar_size, int count)
{
  int min_seats = count / ar_size;
  int seats_left = count - (min_seats * ar_size);
  for (int i = 0; i < ar_size; i++)
  {
    count_ar[i] = (int)min_seats;
    if (seats_left-- > 0) { count_ar[i]++; }
  }
}
