#ifndef DEFINITIONS_H
#define DEFINITIONS_H
/* Defining gnu source */
#define _GNU_SOURCE
/* Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <signal.h>

/* ----- PARAMETERS ----- */

/* energy taken by the master every 1 sec */
#define ENERGY_DEMAND shm_ptr->params[0]
/* initial atoms generated number */
#define N_ATOMI_INIT shm_ptr->params[1]
/* max atomic number possible */
#define N_ATOM_MAX shm_ptr->params[2]
/* minimum atomic number possible under which an atom becomes slag (english for 'scoria') */
#define MIN_N_ATOMICO shm_ptr->params[3]
/* time step for activator to broadcast a signal */
#define STEP_ATTIVATORE shm_ptr->params[4]
/* time step (nanosec) for creating N_NUOVI_ATOMI new atoms */
#define STEP_ALIMENTAZIONE shm_ptr->params[5]
/* new atoms created by power_supplier */
#define N_NUOVI_ATOMI shm_ptr->params[6]
/* max duration time (sec) of the simulation */
#define SIM_DURATION shm_ptr->params[7]
/* energy limit over which the simulation ends in the explode termination case */
#define ENERGY_EXPLODE_THRESHOLD shm_ptr->params[8]

/* ------- CONSTANTS ------- */

/* Number of parameters */
#define N_PARAMS 9
#define MAX_STR_LEN 35

#define SHM_KEY	11
#define MAIN_SEM_KEY 21
#define ACTIV_SEM_KEY 22

#define GENERAL_ERROR_EXIT 255
#define SEM_ERROR_EXIT 254
#define SHM_ERROR_EXIT 253

/* ----- MACROS ----- */

/* It prints the 'str' personalized message. */
#define ERROR(str) fprintf(stderr, "\nError: %s on line %d! PID:%d\n", str, __LINE__, getpid());

#define TEST_ERROR 								\
	if (errno) {								\
		fprintf(stderr, "%s:%d: PID=%5d: Error %d: %s (%s)\n", __FILE__, __LINE__, \
			getpid(), errno, strerrorname_np(errno),  strerror(errno)); 	\
		errno=0;								\
	}

/** Memory managed as:
 * - N_PARAMS params
 * - 2 ints -> activation_num
 * - 2 ints -> split_num
 * - 2 ints -> released_energy
 * - 2 ints -> taken_energy
 * - 2 ints -> slag */
#define ID_SHM ipc_ids[0]

/** Managed as:
 * - 1 coordination semaphore (used at launch)
 * - 4 write on memory semaphore 
 * (can be 0 or 1 for whoever requires to access the memory sections of:
 * 		- activation
 * 		- split
 * 		- released
 * 		- slag)*/
#define ID_MAIN_SEM ipc_ids[1]

/** Set by the activator to a chosen number, released by the atoms */
#define ID_ACTIV_SEM ipc_ids[2]

/* Integer used to to see the state of the simulation end. 
 * Can be modified only if previous value is 0 and only by one process at a time */
#define TERMINATION_STATE shm_ptr->termination_state

/* ------- STRUCTS ------- */

typedef struct _single_stat {
	int total;
	int last;
} single_stat;

/** struct used for the shared memory in the simulation */
typedef struct _memory_map {
	int params[N_PARAMS];
	
	/* n. of activations occurred */
	single_stat activation_num;

	/* n. of splits occurred */
	single_stat split_num;

	/* n. of energy released by the processes */
	single_stat released_energy;

	/* n. of energy taken by the master */
	single_stat taken_energy;

	/* n. of slag made in the simulation */
	single_stat slag;

	/* its value is:
	 * - 0 for timeout (default)
	 * - 1 for explode
	 * - 2 for blackout
	 * - 3 for meltdown	*/
	int termination_state;
} memory_map;

#endif 
