#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>

int readSharedMemory(const char *lockfile, const char *shmem, char *msg, int msg_size) {
    int lock_fd, shmem_fd;

    // Open the lock file
    lock_fd = open(lockfile, O_RDONLY);
    if (lock_fd < 0) {
        perror("Error opening lock file");
        return 0;
    }

    // Acquire an exclusive lock
    if (flock(lock_fd, LOCK_EX) == -1) {
        perror("Locking failed");
        close(lock_fd);
        return 0;
    }

    // Open the shared memory file
    shmem_fd = open(shmem, O_RDONLY);
    if (shmem_fd < 0) {
        perror("Error opening shared memory file");
        flock(lock_fd, LOCK_UN);
        close(lock_fd);
        return 0;
    }

    // Read the message from shared memory
    if (read(shmem_fd, msg, msg_size) != msg_size) {
        perror("Error reading shared memory");
    }

    // Release the lock and close files
    flock(lock_fd, LOCK_UN);
    close(shmem_fd);
    close(lock_fd);
    return msg_size;
}

int writeToSharedMemory(const char *lockfile, const char *shmem, const char *msg, int msg_size) {
// void writeToSharedMemory(unsigned int value) {
    int lock_fd, shmem_fd;

    // Open the lock file, create if it doesn't exist
    lock_fd = open(lockfile, O_WRONLY | O_CREAT, 0666);
    if (lock_fd < 0) {
        perror("Error opening/creating lock file");
        return 0;
    }

    // Acquire an exclusive lock
    if (flock(lock_fd, LOCK_EX) == -1) {
        perror("Locking failed");
        close(lock_fd);
        return 0;
    }

    // Open the shared memory file, create if it doesn't exist
    shmem_fd = open(shmem, O_WRONLY | O_CREAT, 0666);
    if (shmem_fd < 0) {
        perror("Error opening/creating shared memory file");
        flock(lock_fd, LOCK_UN);
        close(lock_fd);
        return 0;
    }

    // Write the value to shared memory
    if (write(shmem_fd, msg, msg_size) != msg_size) {
        perror("Error writing to shared memory");
    }

    // Release the lock and close files
    flock(lock_fd, LOCK_UN);
    close(shmem_fd);
    close(lock_fd);
    return msg_size;
}

// gcc -shared -o shared_memory_lib.so -fPIC shared_memory_lib.c
