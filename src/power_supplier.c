#include "definitions.h"
#include "ipc_lib.h"

/* \# of children forked, incremented every time a fork is executed */
int children_number;
/* id_shm, id_main_sem, id_activator_sem */
int ipc_ids[3];
memory_map* shm_ptr;

void signal_handler(int signo) {
    switch(signo) {
        case SIGTERM:
            /* changing external flag */
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
	int i, status, fatal_error;
	char **children_argv;
	struct sigaction sa_pow_supplier;
	
	/* link to resources */
	sem_linking(&(ID_MAIN_SEM), &(ID_ACTIV_SEM));
	ID_SHM = shm_find();
	shm_ptr = shm_attach(ID_SHM);

	/* initial operations */
    fatal_error = 0;
    children_number = 0;
	sa_pow_supplier.sa_flags = 0;
	sa_pow_supplier.sa_handler = signal_handler;
	sigemptyset(&sa_pow_supplier.sa_mask);
	sigaction(SIGALRM, &sa_pow_supplier, NULL);
	sigaction(SIGTERM, &sa_pow_supplier, NULL);
    srand(time(NULL));
    children_argv = malloc(sizeof(char*) * 2);
    children_argv[0] = malloc(sizeof(char) * MAX_STR_LEN);
    children_argv[0] = strcpy(children_argv[0], "./atom.out");
    children_argv[1] = NULL;

	/* semaphore wait_for_zero: start of the simulation */
    sem_reserve(ID_MAIN_SEM, 0);
	sem_wait_zero(ID_MAIN_SEM, 0);

	/* main cycle */


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
    printf("%s: IPC resources closed.\n", __FILE__);

    /* free() of the dynamic memory */
    free(children_argv[0]);
    TEST_ERROR
    free(children_argv);
    TEST_ERROR
    printf("%s: Memory free()s executed.\n", __FILE__);
}
