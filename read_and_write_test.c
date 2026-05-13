#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_PATH "/dev/bastea-driver"

void read_device(void) {
    int fd;
    char c;

    fd = open(DEVICE_PATH, O_RDONLY);

    if (fd < 0) {
        perror("Failed to open device");
        return;
    }

    while (read(fd, &c, 1) > 0) {
        printf("%c", c);
    }

    close(fd);
}

int main(void) {
    int fd;
    const char *message = "Message written from C test program\n";

    printf("Initial buffer contents:\n");
    read_device();

    fd = open(DEVICE_PATH, O_WRONLY);

    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("\nWriting new message to device...\n");

    write(fd, message, strlen(message));

    close(fd);

    printf("\nUpdated buffer contents:\n");
    read_device();

    return 0;
}