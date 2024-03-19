#include "ipc_lib.h"

int sem_create(key_t key, int nsems) {
    int id;
    if ((id = semget(key, nsems, IPC_CREAT | 0666)) == -1) {
        ERROR("creating the semaphore")
        exit(SEM_ERROR_EXIT);
    }
    return id;
}

void sem_allocate(int *id_main_sem, int *id_activ_sem){
    printf("- main_sem created with id: %d\n", *id_main_sem = sem_create(MAIN_SEM_KEY, 5)); /* 1 for coordination + 4 for stats */
    printf("- activ_sem created with id: %d\n", *id_activ_sem = sem_create(ACTIV_SEM_KEY, 1));
}

int sem_find(key_t key, int nsems) {
    int id;
    if ((id = semget(key, nsems, S_IWUSR | S_IRUSR)) == -1) {
        ERROR("finding the semaphore")
        exit(SEM_ERROR_EXIT);
    }
    return id;
}

void sem_linking(int *id_main_sem, int *id_activ_sem){
    printf("agent: %s\n", __FILE__);
    printf("- main_sem linked to id: %d\n", *id_main_sem = sem_find(MAIN_SEM_KEY, 5));
    printf("- activ_sem linked to id: %d\n", *id_activ_sem = sem_find(ACTIV_SEM_KEY, 1));
}

void sem_reserve(int semid, int sem_num) {
    struct sembuf sops;
    sops.sem_num = sem_num;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    while (semop(semid, &sops, 1) == -1 && errno == EINTR) {
        ERROR("re-iterating")
    }
}

void sem_release(int semid, int sem_num) {
    struct sembuf sops;
    sops.sem_num = sem_num;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    while (semop(semid, &sops, 1) == -1 && errno == EINTR) {
        ERROR("sem_release")
    }
}

void sem_wait_zero(int id, int sem_num) {
    struct sembuf sops;
    sops.sem_num = sem_num;
    sops.sem_op = 0;
    sops.sem_flg = 0;
    errno = 0;
    while(semop(id, &sops, 1) == -1 && errno == EINTR){
        errno = 0;
    }
}

void sem_set_val(int id, int sem_num, int val) {
    if (semctl(id, sem_num, SETVAL, val) == -1) {
        ERROR("sem_set_val");
        exit(SEM_ERROR_EXIT);
    }
}

void sem_set_all(int id, int value, int arr_size){
    int semval, i;
    union semun arg;
    arg.array = malloc(arr_size * sizeof(int));
    for(i = 0; i < arr_size; i++){
        arg.array[i] = value;
    }
    semval = semctl(id, 0, SETALL, arg);
    TEST_ERROR
    if (semval == -1) {
        ERROR("sem_set_all");
        exit(SEM_ERROR_EXIT);
    }
}

int sem_get_val(int id, int sem_num) {
    int semval = semctl(id, sem_num, GETVAL);
    if (semval == -1) {
        ERROR("sem_get_val");
        exit(SEM_ERROR_EXIT);
    }
    return semval;
}

void sem_destroy(int id) {
    if (semctl(id, 0, IPC_RMID) == -1) {
        ERROR("sem_destroy");
        exit(SEM_ERROR_EXIT);
    }
}

/* -------------------------- */

int shm_allocate() {
    int id;
    if ((id = shmget(SHM_KEY, sizeof(memory_map), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1) {
        ERROR("shm_allocate");
        exit(SHM_ERROR_EXIT);
    }
    return id;
}

int shm_find() {
    int id;
    if ((id = shmget(SHM_KEY, sizeof(memory_map), S_IRUSR | S_IWUSR)) == -1) {
        ERROR("shm_find");
        exit(SHM_ERROR_EXIT);
    }
    return id;
}

memory_map *shm_attach(int id) {
    void *shm_ptr;
    if ((shm_ptr = shmat(id, NULL, 0)) == (void *)-1) {
        ERROR("shm_attach");
        exit(SHM_ERROR_EXIT);
    }
    return shm_ptr;
}

void shm_detach(memory_map *shm_ptr) {
    if (shmdt(shm_ptr) == -1) {
        ERROR("shm_detach");
        exit(SHM_ERROR_EXIT);
    }
}

void shm_destroy(int id) {
    if (shmctl(id, IPC_RMID, NULL) == -1) {
        ERROR("shm_destroy");
        exit(SHM_ERROR_EXIT);
    }
}

