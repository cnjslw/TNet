#include <stdio.h>
#include "tnet_tiny.h"

int main (void) {
    tnet_init();
    printf("tnet running\n");
    while (1) {
        tnet_poll();
    }

    return 0;
}
