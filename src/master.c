#include "definitions.h"
#include "ipc_lib.h"
#include "master_lib.h"

/* \# of children forked, incremented every time a fork is executed */
int children_number;
/* id_shm, id_main_sem, id_activator_sem */
int ipc_ids[3];
memory_map* shm_ptr;
int termination;

void signal_handler(int signo) {
    switch(signo) {
        case SIGTERM:
            /* changing external flag */
            print_termination(TERMINATION_STATE);
            termination = 1;
            break;
        case SIGALRM:
            break;
        default:
            ERROR("default case in signal_handler")
            exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
	/* declarations */
	int i, file_line_number, status, fatal_error;
	FILE *config_file;
    char **children_argv;
    struct sigaction sa_master;

    /* initial operations */
    termination = 0;
    fatal_error = 0;
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
    
    /* control parameters input */
    i = 0;
    switch((i = invalid_params(shm_ptr))){
        case 0:
            break;
        case 1: 
            ERROR("invalid param. value")
            fatal_error = -3;
            break;
        default:
            ERROR("multiple invalid param. values")
            fprintf(stderr, "Found %d values out of range.\n", i);
            fatal_error = -3;
            break;
    }

    /* setting the semaphores */
    sem_set_all(ID_MAIN_SEM, 1, 5);
    sem_set_val(ID_MAIN_SEM, 0, (N_ATOMI_INIT + 2));  /* to synch the children */

    /* setting up children */
    if(setpgid(0, 0)){
        ERROR("set_group failed")
        TEST_ERROR
        fatal_error = -2;
    }

    while(children_number < N_ATOMI_INIT + 2 && !fatal_error) {
        pid_t child_pid;
        switch(child_pid = fork()) {
            case -1:
                ERROR("fork() failed")
                TEST_ERROR
                fatal_error = -1;
                killpg(0, SIGTERM); /* sends SIGNO to all current group processes */
                TEST_ERROR
                break;
            case 0:
                if (children_number == 0) {    
                    /* power_supplier */
                    children_argv[0] = strcpy(children_argv[0], "./bin/power_supplier.out");
                    execv("./bin/power_supplier.out", children_argv);
                    ERROR("execv failed")
                    TEST_ERROR
                    fatal_error = 1;
                } else if (children_number == 1) { 
                    /* activator */
                    children_argv[0] = strcpy(children_argv[0], "./bin/activator.out");
                    execv("./bin/activator.out", children_argv);
                    ERROR("execv failed")
                    TEST_ERROR
                    fatal_error = 2;
                } else {    
                    /* atom */
                    children_argv[0] = strcpy(children_argv[0], "./bin/atom.out");
                    execv("./bin/atom.out", children_argv);
                    ERROR("execv failed")
                    TEST_ERROR
                    fatal_error = 3 + children_number;
                }
                break;
            default:
                /* master */
                if(child_pid != -1)
                    children_number++;
                break;
        }
    }

    if(fatal_error) {
        fprintf(stderr, "\nError occurred in the \"execv()\" execution. fatal_error = %d\n\n", 
            fatal_error);
        TEST_ERROR
        killpg(0, SIGTERM);
        TEST_ERROR
    } else {
        /* preparation for main cycle */
        sa_master.sa_handler = signal_handler;
        sa_master.sa_flags = 0;
        sigemptyset(&(sa_master.sa_mask));
        sigaction(SIGALRM, &sa_master, NULL);
        sigaction(SIGTERM, &sa_master, NULL);
        sem_wait_zero(ID_MAIN_SEM, 0);
    
        /* main cycle */
        /* @todo loop on *termination* condition */
        
    }

    /* waiting children status */
    for(i = 0; i < children_number; i++){
        if(!WIFEXITED(status))
            fprintf(stderr, "\nChild returned with status: %d.\n", status);
        if (errno != EINTR)
            i++;
		status = 0;
    }
    if(children_number)
        printf("\n%s: All %d children terminated.\n", __FILE__, children_number);

    /* ending closures */
    shm_detach(shm_ptr);
    TEST_ERROR
    shm_destroy(ID_SHM);
    TEST_ERROR
    sem_destroy(ID_MAIN_SEM);
    TEST_ERROR
    sem_destroy(ID_ACTIV_SEM);
    TEST_ERROR
    printf("%s: IPC resources closed.\n", __FILE__);

    /* free() of the dynamic memory */
    free(children_argv[0]);
    TEST_ERROR
    free(children_argv);
    TEST_ERROR
    printf("%s: Memory free()s executed.\n", __FILE__);
}
