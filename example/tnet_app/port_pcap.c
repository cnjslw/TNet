#include "pcap_device.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "tnet_tiny.h"

static pcap_t* pcap;

// pcap所用的网卡
const char* ip_str = "192.168.74.1";
const char my_mac_addr[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

/**
 * 初始化网络驱动
 */
tnet_err_t tnet_driver_open(uint8_t * mac_addr)
{
	memcpy(mac_addr , my_mac_addr,sizeof(my_mac_addr));
	pcap = pcap_device_open(ip_str , mac_addr , 1);
	if (pcap == (pcap_t*)0)
	{
		exit(-1);
	}
	return TNET_ERR_OK;
}

/**
 * 发送数据
 */
tnet_err_t tnet_driver_send(tnet_packet_t* packet) {
	return pcap_device_send(pcap, packet->data, packet->size);
}

/**
 * 读取数据
 */
tnet_err_t tnet_driver_read(tnet_packet_t ** packet)
{
	uint16_t size;
	tnet_packet_t* r_packet = tnet_alloc_for_read(TNET_CFG_PACKET_MAX_SIZE);
	size = pcap_device_read(pcap , r_packet->data , TNET_CFG_PACKET_MAX_SIZE);
	if (size)
	{
		r_packet->size = size;
		*packet = r_packet;
		return TNET_ERR_OK;
	}
	return TNET_ERR_IO;
}

/*
* 获取程序启动以来的时间
*/
const tnet_time_t tsys_get_time(void)
{
	static uint32_t pre = 0;
#if 0
	uint32_t c = clock() / CLOCKS_PER_SEC;
	if (c != pre)
	{
		printf("sec : %d , 100ms : %d \n" , c , (tnet_time_t)(clock()*10/CLOCKS_PER_SEC));
	}
#endif
	return (tnet_time_t)(clock() / CLOCKS_PER_SEC);
}