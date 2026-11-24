
#include <stdio.h>
#include <iostream>
#include <string>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

pthread_spinlock_t lock[5];
pthread_spinlock_t monitor_lock;

void *worker1(void *arg)
{
    int i = atoi((char *)arg + 6);
    printf("%s 시작 \t %d\n", (char *)arg, i);

    if (i == 5)
    {
        // 오른쪽 포크 먼저
        pthread_spin_lock(&lock[i % 5]);
        printf("%d 철학자 오른쪽 포크 획득\n", i);
        sleep(3);

        // 왼쪽 포크 나중에
        pthread_spin_lock(&lock[i - 1]);
        printf("%d 철학자 왼쪽 포크 획득\n", i);

        sleep(3);

        printf("%d 철학자 식사 끝\n", i);

        pthread_spin_unlock(&lock[i - 1]);
        pthread_spin_unlock(&lock[i % 5]);
    }
    else
    {
        // 왼쪽 포크 먼저
        pthread_spin_lock(&lock[i - 1]);
        printf("%d 철학자 왼쪽 포크 획득\n", i);
        sleep(3);

        // 오른쪽 포크 나중에
        pthread_spin_lock(&lock[i % 5]);
        printf("%d 철학자 오른쪽 포크 획득\n", i);

        sleep(3);

        printf("%d 철학자 식사 끝\n", i);

        pthread_spin_unlock(&lock[i - 1]);
        pthread_spin_unlock(&lock[i % 5]);
    }

    return NULL;
}

void *worker2(void *arg)
{
    return NULL;
}

int main()
{
    char *name[] = {"철학1", "철학2", "철학3", "철학4", "철학5"};
    pthread_t tid[5];
    pthread_attr_t attr[5];

    for (int i = 0; i < 5; i++)
    {
        pthread_spin_init(&lock[i], PTHREAD_PROCESS_PRIVATE);
        pthread_attr_init(&attr[i]);
        pthread_create(&tid[i], &attr[i], worker1, name[i]);
    }

    for (int i = 0; i < 5; i++)
        pthread_join(tid[i], NULL);

    for (int i = 0; i < 5; i++)
    {
        pthread_spin_destroy(&lock[i]);
        pthread_attr_destroy(&attr[i]);
    }

    return 0;
}
