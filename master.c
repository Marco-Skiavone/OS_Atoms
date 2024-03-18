#include "definitions.h"
#include "ipc_lib.h"

/* \# of children forked, incremented every time a fork is executed */
int children_number;
/* id_shm, id_main_sem, id_activator_sem */
int ipc_ids[3];
memory_map* shm_ptr;

void signal_handler(int signo) {
    int i;
    switch(signo) {
        case SIGTERM:
            shm_detach(shm_ptr);
            /* changing external flag */
            break;
        default:
            ERROR("default case in signal_handler")
            exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
	/* declarations */
	int i, j, file_line_number, status;
	FILE *config_file;
    char **children_argv;

    /* initial operations */
    children_number = 0;
	if(argc != 2){
		fprintf(stderr, "Invalid argument number. Choose a configuration set to launch the simulation.\n");
		exit(EXIT_FAILURE);
	}
    srand(time(NULL));
    children_argv = malloc(sizeof(char*) * 2);
    children_argv[0] = malloc(sizeof(char) * MAX_STR_LEN);
    children_argv[1] = NULL;

	/* allocate resources */
	sem_allocate(&(ID_MAIN_SEM), &(ID_ACTIV_SEM));
	ID_SHM = shm_allocate();
	shm_ptr = shm_attach(ID_SHM);

    /* setting the semaphores */
    printf("id sem: %d\n", ID_MAIN_SEM);
    sem_set_all(ID_MAIN_SEM, 1, 5);
    sem_set_val(ID_MAIN_SEM, 0, N_ATOMI_INIT + 2);  /* to synch the children */

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

    /* setting up children */

    /* main cycle */

    /* waiting children status */
    for(i = 0; i < children_number; i ++){
        wait(&status);
        TEST_ERROR
        if(status != 0)
            fprintf(stderr, "\nChild returned with status: %d.\n", status);
    }

    /* ending closures */
    shm_destroy(ID_SHM);
    TEST_ERROR
    sem_destroy(ID_MAIN_SEM);
    TEST_ERROR
    sem_destroy(ID_ACTIV_SEM);
    TEST_ERROR

    /* free() of the dynamic memory */
    free(children_argv[0]);
    TEST_ERROR
    free(children_argv);
    TEST_ERROR
}
