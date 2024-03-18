#include "definitions.h"
#include "ipc_lib.h"

/* \# of children forked, incremented every time a fork is executed */
int children_number;
/* id_shm, id_main_sem, id_activator_sem */
int ipc_ids[3];
memory_map* shm_ptr;

int main(int argc, char* argv[]) {
	int i, j, file_line_number;
	FILE *config_file;
	/* end of declarations */
	if(argc != 2){
		fprintf(stderr, "Invalid argument number. Choose a configuration set to launch the simulation.\n");
		exit(EXIT_FAILURE);
	}
	/* allocate resources */
	sem_allocate(&(ID_MAIN_SEM), &(ID_ACTIV_SEM));
	ID_SHM = shm_allocate();
	shm_ptr = shm_attach(ID_SHM);

    /* setting the semaphores */
    printf("id sem: %d\n", ID_MAIN_SEM);
    sem_set_all(ID_MAIN_SEM, 1, 5);
    sem_set_val(ID_MAIN_SEM, 0, N_ATOMI_INIT + 2);
	/* read the file to set the parameters */
	file_line_number = atoi(argv[1]);
    TEST_ERROR
    if(file_line_number < 1){
		fprintf(stderr, "Invalid argument number. Choose a VALID configuration set to launch the simulation.\n");
        exit(EXIT_FAILURE);
    }
    config_file = fopen("configurations.txt", "r");
	TEST_ERROR
    for(i = 0 ; i < file_line_number; ){
		char c = fgetc(config_file);
        if(c == '\n'){
            i++;
        } else if(c == EOF){
            fprintf(stderr, "Invalid argument number. Choose a VALID configuration set to launch the simulation.\n");
            fclose(config_file);
            exit(EXIT_FAILURE);
        }
    }
    for(i = 0; i < N_PARAMS; i++){
        if(fscanf(config_file, "%d", &(shm_ptr->params[i])) == 0){
            ERROR("reading params")
            fclose(config_file);
            exit(EXIT_FAILURE);
        }
    }
    fclose(config_file);
	TEST_ERROR

    printf("\nParams:\nENERGY_DEMAND: %d,\nN_ATOMI_INIT: %d,\nN_ATOM_MAX: %d,\nMIN_N_ATOMICO: %d,\nSTEP_ATTIVATORE: %d,\nSTEP_ALIMENTAZIONE: %d,\nN_NUOVI_ATOMI: %d,\nSIM_DURATION: %d,\nENERGY_EXPLODE_THRESHOLD: %d\n\n", 
        ENERGY_DEMAND, N_ATOMI_INIT, N_ATOM_MAX, MIN_N_ATOMICO, STEP_ATTIVATORE, STEP_ALIMENTAZIONE, N_NUOVI_ATOMI, SIM_DURATION, ENERGY_EXPLODE_THRESHOLD);

}
