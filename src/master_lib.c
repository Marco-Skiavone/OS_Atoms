#include "master_lib.h"

int invalid_params(memory_map *shm_ptr){
	int i, exit = 0;
	if(shm_ptr == NULL)
		return -1;
	for(i = 0; i < N_PARAMS && !exit; i++)
		if(i != 7 && i != 8 && shm_ptr->params[i] < 0)
			exit++;
	if(N_ATOM_MAX < MIN_N_ATOMICO)
		exit++;
	if(SIM_DURATION < 2)
		exit++;
	if(ENERGY_EXPLODE_THRESHOLD < 100)
		exit++;
	return exit;
}
