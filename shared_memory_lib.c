#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <stdint.h>
#include <string.h>

int readSharedMemory(const char *lockfile, const char *shmem, char *msg, int msg_size) {
    // msg_size 读取成功
    // -1 打开lockfile失败
    // -2 上锁失败
    // -3 打开shmem失败
    // -4 读取shmem失败
    int lock_fd, shmem_fd;

    // Open the lock file
    lock_fd = open(lockfile, O_RDONLY);
    if (lock_fd < 0) {
        perror("Error opening lock file");
        return -1;
    }

    // Acquire an exclusive lock
    if (flock(lock_fd, LOCK_EX) == -1) {
        perror("Locking failed");
        close(lock_fd);
        return -2;
    }

    // Open the shared memory file
    shmem_fd = open(shmem, O_RDONLY);
    if (shmem_fd < 0) {
        perror("Error opening shared memory file");
        flock(lock_fd, LOCK_UN);
        close(lock_fd);
        return -3;
    }

    // Read the message from shared memory
    if (read(shmem_fd, msg, msg_size) != msg_size) {
        perror("Error reading shared memory");
        return -4;
    }

    // Release the lock and close files
    flock(lock_fd, LOCK_UN);
    close(shmem_fd);
    close(lock_fd);
    return msg_size;
}

int writeToSharedMemory(const char *lockfile, const char *shmem, const char *msg, int msg_size) {
    // msg_size 写入成功
    // -1 打开锁文件失败
    // -2 上锁失败
    // -3 打开shmem失败
    // -4 写入shmem失败
    int lock_fd, shmem_fd;

    // Open the lock file, create if it doesn't exist
    lock_fd = open(lockfile, O_WRONLY | O_CREAT, 0666);
    if (lock_fd < 0) {
        perror("Error opening/creating lock file");
        return -1;
    }

    // Acquire an exclusive lock
    if (flock(lock_fd, LOCK_EX) == -1) {
        perror("Locking failed");
        close(lock_fd);
        return -2;
    }

    // Open the shared memory file, create if it doesn't exist
    shmem_fd = open(shmem, O_WRONLY | O_CREAT, 0666);
    if (shmem_fd < 0) {
        perror("Error opening/creating shared memory file");
        flock(lock_fd, LOCK_UN);
        close(lock_fd);
        return -3;
    }

    // Write the value to shared memory
    if (write(shmem_fd, msg, msg_size) != msg_size) {
        perror("Error writing to shared memory");
        return -4;
    }

    // Release the lock and close files
    flock(lock_fd, LOCK_UN);
    close(shmem_fd);
    close(lock_fd);
    return msg_size;
}


int checkAndWrite(const char *lockfile, const char *shmem, const char *condition, const char *msg, int msg_size) {
    // msg_size shmem内容和condition相等，并且写入成功
    // 0 shmem内容和condition不相等
    // -1 无法打开锁文件
    // -2 上锁失败
    // -3 无法打开shmem
    // -4 无法读取shmem
    // -5 无法写入shmem
    int lock_fd, shmem_fd;
    char buff[1024];

    // Open the lock file
    lock_fd = open(lockfile, O_RDONLY);
    if (lock_fd < 0) {
        perror("Error opening lock file");
        return -1;
    }

    // Acquire an exclusive lock
    if (flock(lock_fd, LOCK_EX) == -1) {
        perror("Locking failed");
        close(lock_fd);
        return -2;
    }

    // Open the shared memory file
    shmem_fd = open(shmem, O_RDWR);
    if (shmem_fd < 0) {
        perror("Error opening shared memory file");
        flock(lock_fd, LOCK_UN);
        close(lock_fd);
        return -3;
    }

    // Read the message from shared memory
    if (read(shmem_fd, buff, msg_size) != msg_size) {
        perror("Error reading shared memory");
        return -4;
    }

    if (strncmp(buff, condition, msg_size) != 0) {
        return 0;
    }

    if (write(shmem_fd, msg, msg_size) != msg_size) {
        perror("Error writing shared memory");
        return -5;
    }

    // Release the lock and close files
    flock(lock_fd, LOCK_UN);
    close(shmem_fd);
    close(lock_fd);
    return msg_size;
}

// gcc -shared -o shared_memory_lib.so -fPIC shared_memory_lib.c
