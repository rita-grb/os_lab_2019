  #include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    /* Создание дочернего процесса */
    pid_t  child_pid = fork();
    if (child_pid > 0)
    {
        /* Родительский процесс, делаем паузу */
        sleep(60);
    }
    else
    {
        /* Дочерний процесс, немедленно завершаем его работу */
        exit(0);
    }
    return 0;
}
