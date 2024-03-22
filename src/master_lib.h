#ifndef MASTER_LIB_H
#define MASTER_LIB_H
#include "definitions.h"

/* Return the \# of invalid parameters found */
int invalid_params(memory_map *shm_ptr);

/* Print the final termination state */
void print_termination(int state);

#endif
