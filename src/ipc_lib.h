#ifndef IPC_LIB_H

#define IPC_LIB_H
#include "definitions.h"

union semun {
	int val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

/* Create a semaphore set, given key and \# of semaphores */
int sem_create(key_t key, int nsems);

/* Call all the sem_creates set, called **only** by the master */
void sem_allocate(int *id_main_sem, int *id_activ_sem);

/* Attach to a previously created sem. set */
int sem_find(key_t key, int nsems);

/* Call the sem_find to attach to the created semaphores */
void sem_linking(int *id_main_sem, int *id_activ_sem);

/* Execute the "reserve": blocking untill -1 operation is possible */
void sem_reserve(int id, int sem_num);

/* Execute the "release": doing +1 operation */
void sem_release(int id, int sem_num);

/* Execute the "waitforzero", blocking untill the sem. value is 0 */
void sem_wait_zero(int id, int sem_num);

/* Set the value of the sem. given, to "val" */
void sem_set_val(int id, int sem_num, int val);

/* Set the value of all the semaphores in the set given, to "value" */
void sem_set_all(int id, int value, int arr_size);

/* Destroy the semaphore set. */
void sem_destroy(int id);

/* Return the value of the semaphore passed as argument */
int sem_get_val(int id, int sem_num);

/* ----------------------------- */

/* Create shm and returns the id */
int shm_allocate();

/* Return id of the shm */
int shm_find();

/* Attach the shm to the id, return a pointer to the shm */
memory_map *shm_attach(int id);

/* Detach from the shm, given its pointer */
void shm_detach(memory_map *shm_ptr);

/* Destroy the shm, given its id */
void shm_destroy(int id);

#endif
