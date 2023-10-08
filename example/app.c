#include <stdio.h>
#include "xnet_tiny.h"
#include "xserver_datetime.h"

int main(void) {
    xnet_init();

    xserver_datetime_create;
    printf("xnet running\n");
    while (1) {
        xnet_poll();
    }

    return 0;
}
