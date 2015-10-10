
#include "util.h"

void freak_log(const char *fmt, ...) {
#ifndef FREAK_NO_LOG
  va_list ap;
  FILE *log_file = fopen("freak.log", "a");

  va_start(ap, fmt);
  vfprintf(log_file, fmt, ap);
  va_end(ap);

  fprintf(log_file, "\n");
  fflush(log_file);
  fclose(log_file);
#endif
}
