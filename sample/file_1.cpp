#include <stdio.h>
#include <iostream>
#include <string>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

int sum = 0;

typedef struct {
    pthread_spinlock_t lock1;
    pthread_mutex_t lock2;
    bool spin;
    time_t start_time;
} hybrid;

hybrid *hybrid_lock;

hybrid *create() {
    hybrid *lock = (hybrid *)malloc(sizeof(hybrid));

    pthread_spin_init(&(lock->lock1), PTHREAD_PROCESS_PRIVATE);
    pthread_mutex_init(&(lock->lock2), NULL);

    lock->spin = true;

    return lock;
}

void pthread_hybrid_lock(hybrid *lock) {
    time(&(lock->start_time));

    while (pthread_spin_trylock(&(lock->lock1)) == EBUSY) {
        time_t cur;
        time(&cur);

        if (difftime(cur, lock->start_time) > 5) {
            cout << "spinlock -> mutex" << endl;

            pthread_mutex_lock(&(lock->lock2));
            break;
        }
    }
}

void pthread_hybrid_unlock(hybrid *lock) {
    if (lock->spin) {
        pthread_spin_unlock(&(lock->lock1));
    }
    else {
        pthread_mutex_unlock(&(lock->lock2));
    }
}

void *worker(void* arg) {
    printf("%s 시작 \t %d\n", (char*)arg, sum);

    for (int i = 0; i < 1000000; i++) {
        pthread_hybrid_lock(hybrid_lock);

        if (i == 0)
            sleep(10);

        sum = sum + 10;

        pthread_hybrid_unlock(hybrid_lock);
    }

    printf("%s 끝 \t %d\n", (char*)arg, sum);

    return NULL;
}

int main() {
    char *name[] = { (char*)"황기태", (char*)"이찬수" };

    pthread_t tid[2];
    pthread_attr_t attr[2];

    pthread_attr_init(&attr[0]);
    pthread_attr_init(&attr[1]);

    hybrid_lock = create();

    pthread_create(&tid[0], &attr[0], worker, name[0]);
    pthread_create(&tid[1], &attr[1], worker, name[1]);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    printf("sum = %d\n", sum);

    pthread_spin_destroy(&(hybrid_lock->lock1));
    pthread_mutex_destroy(&(hybrid_lock->lock2));

    free(hybrid_lock);

    return 0;
}