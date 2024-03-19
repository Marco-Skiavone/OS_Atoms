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
	int i, status, child_pid, fork_error;
	char **children_argv;
	struct sigaction sa_pow_supplier;
	/* end of declarations */
	
	/* link to resources */
	sem_linking(&(ID_MAIN_SEM), &(ID_ACTIV_SEM));
	ID_SHM = shm_find();
	shm_ptr = shm_attach(ID_SHM);

	/* final adjustments */
	sa_pow_supplier.sa_flags = 0;
	sa_pow_supplier.sa_handler = signal_handler;
	sigemptyset(&sa_pow_supplier.sa_mask);
	sigaction(SIGALRM, &sa_pow_supplier, NULL);
	sigaction(SIGTERM, &sa_pow_supplier, NULL);

	/* semaphore wait_for_zero: start of the simulation */
	sem_wait_zero(ID_MAIN_SEM, 0);

	/* main cycle */


	/* waiting children status */
    for(i = 0; i < children_number; i++){
        wait(&status);
        TEST_ERROR
        if(status != 0)
            fprintf(stderr, "\nChild returned with status: %d.\n", status);
    }
    printf("\nAll starting children terminated.\n");

    /* ending closures */
    shm_destroy(ID_SHM);
    TEST_ERROR
    sem_destroy(ID_MAIN_SEM);
    TEST_ERROR
    sem_destroy(ID_ACTIV_SEM);
    TEST_ERROR
    printf("IPC resources closed.\n");

    /* free() of the dynamic memory */
    free(children_argv[0]);
    TEST_ERROR
    free(children_argv);
    TEST_ERROR
    printf("Memory free()s executed.\n");
}
