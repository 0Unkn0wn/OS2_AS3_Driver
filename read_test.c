#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd;
    char c;

    fd = open("/dev/bastea-driver", O_RDONLY);

    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    while (read(fd, &c, 1) > 0) {
        printf("%c", c);
    }

    close(fd);

    return 0;
}