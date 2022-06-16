#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#define EVENT_BUF_NUM 64

int main(void) {
 int i, quit = 1;
 int fd = -1;

 size_t read_bytes;
    struct input_event event_buf[EVENT_BUF_NUM];


    int fd = open("/dev/input/event2", O_RDONLY);

    if(fd < 0) {
        printf("application : keypad driver open fail!\n");
        exit(1);
    }

    read(fd, event_buf, sizeof(struct input_event) * EVENT_BUF_NUM);

    printf("press the key button!\n");

    int quit = 1;
    int i;
    while(quit) {
        read_bytes = read(fd, event_buf, (sizeof(struct input_event)*EVENT_BUF_NUM) );
        for(i=0; i<(read_bytes/sizeof(struct input_event)); i++ ) {
            if ((event_buf[i].type == EV_KEY) && (event_buf[i].value == 0)) {
                printf("\n  Button key : %d\n", event_buf[i].code);
                
                if(event_buf[i].code == 16) {
                    quit = 0;
                }
            }
        }
    }
    close(fd);
    return 0;
}
