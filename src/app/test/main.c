/**
 * @brief 测试程序
 * @version 0.1
 */

#include "sys_plat.h"
#include <stdio.h>

static sys_sem_t sem;

void thread1_entry(void* arg)
{
    while (1) {
        plat_printf("this is thread1: %s \n", (char*)arg);
        sys_sleep(1000);
    }
}

void thread2_entry(void* arg)
{
    while (1) {
        sys_sem_wait(sem, 0);
        plat_printf("this is thread2: %s \n", (char*)arg);
        sys_sleep(1000);
    }
}

int main(void)
{
    sys_sem_t sem = sys_sem_create(0);
    sys_thread_create(thread1_entry, "AAA");
    sys_thread_create(thread2_entry, "BBB");

    pcap_t* pcap = pcap_device_open(netdev0_phy_ip, netdev0_hwaddr);
    while (pcap) {
        static uint8_t buffer[1024];
        static int counter = 0;
        struct pcap_pkthdr* pkthdr;
        const uint8_t* pkt_data;

        plat_printf("begin test : %d\n", counter++);
        for (int i = 0; i < sizeof(buffer); i++)
            buffer[i] = i;

        if (pcap_next_ex(pcap, &pkthdr, &pkt_data) != 1) {
            continue;
        }

        int len = pkthdr->len > sizeof(buffer) ? sizeof(buffer) : pkthdr->len;
        plat_memcpy(buffer, pkt_data, len);

        buffer[0] = 1;
        buffer[1] = 2;

        if (pcap_inject(pcap, buffer, sizeof(buffer)) == -1) {
            plat_printf("pcap send : send packet fail %s\n", pcap_geterr(pcap));
            break;
        }
        sys_sleep(10);
    }
    printf("Hello TNet");
    return 0;
}
