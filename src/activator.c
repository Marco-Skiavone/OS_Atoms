#include "definitions.h"
#include "ipc_lib.h"

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
	struct sigaction sa_pow_supplier;
	
	/* link to resources */
	sem_linking(&(ID_MAIN_SEM), &(ID_ACTIV_SEM));
	ID_SHM = shm_find();
	shm_ptr = shm_attach(ID_SHM);

	/* initial operations */
    fatal_error = 0;
	sa_pow_supplier.sa_flags = 0;
	sa_pow_supplier.sa_handler = signal_handler;
	sigemptyset(&sa_pow_supplier.sa_mask);
	sigaction(SIGALRM, &sa_pow_supplier, NULL);
	sigaction(SIGTERM, &sa_pow_supplier, NULL);
    srand(time(NULL));

	/* semaphore wait_for_zero: start of the simulation */
	sem_reserve(ID_MAIN_SEM, 0);
	sem_wait_zero(ID_MAIN_SEM, 0);

	/* main cycle */


    /* ending closures */
    shm_detach(shm_ptr);
    TEST_ERROR
    printf("%s: IPC resources closed.\n", __FILE__);

    /* free() of the dynamic memory */
	/* @todo ...nothing for now... */
    printf("%s: Memory free()s executed.\n", __FILE__);
	
}
