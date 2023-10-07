#ifndef TNET_TINY_H
#define TNET_TINY_H
#include <stdint.h>

#define TNET_CFG_NETIF_IP               {192, 168, 74, 2}  // 本机网卡IP
#define TNET_CFG_PACKET_MAX_SIZE 1516 //收发数据的最大大小
#define TARP_CFG_ENTRY_OK_TMO	        (5) // ARP表项超时时间
#define TARP_CFG_ENTRY_PENDING_TMO	    (1) // ARP表项挂起超时时间
#define TARP_CFG_MAX_RETRIES		    4 // ARP表挂起时重试查询次数

#pragma pack(1)
#define TNET_IPV4_ADDR_SIZE 4 //IP地址长度
#define TNET_MAC_ADDR_SIZE 6 //MAC长度

//****************************以太网数据帧************************************
typedef struct _tether_hdr_t {
	uint8_t dest[TNET_MAC_ADDR_SIZE];//目标mac地址
	uint8_t src[TNET_MAC_ADDR_SIZE];//源mac地址
	uint16_t protocol;//协议
}tether_hdr_t;

//****************************ARP数据包**************************************

#define TARP_HW_ETHER               0x1         // 以太网
#define TARP_REQUEST                0x1         // ARP请求包
#define TARP_REPLY                  0x2         // ARP响应包

typedef struct _tarp_packet_t {
	uint16_t hw_type, pro_type;                 // 硬件类型和协议类型
	uint8_t hw_len, pro_len;                    // 硬件地址长 + 协议地址长
	uint16_t opcode;                            // 请求/响应
	uint8_t sender_mac[TNET_MAC_ADDR_SIZE];     // 发送包硬件地址
	uint8_t sender_ip[TNET_IPV4_ADDR_SIZE];     // 发送包协议地址
	uint8_t target_mac[TNET_MAC_ADDR_SIZE];     // 接收方硬件地址
	uint8_t target_ip[TNET_IPV4_ADDR_SIZE];     // 接收方协议地址
}tarp_packet_t;
#pragma pack()

//****************************返回宏**************************************

typedef enum _tnet_err_t {
	TNET_ERR_OK = 0,
	TNET_ERR_IO = -1,
}tnet_err_t;

//****************************网络数据包**********************************

typedef struct _tnet_packet_t {
	uint16_t size; //包中有效数据的大小
	uint8_t* data;//包的数据起始位置
	uint8_t payload[TNET_CFG_PACKET_MAX_SIZE];//最大负载数据量
}tnet_packet_t;



typedef uint32_t tnet_time_t;
const tnet_time_t tsys_get_time(void);
int tnet_check_tmo(tnet_time_t * time , uint32_t sec);

tnet_packet_t* tnet_alloc_for_send(uint16_t data_size);
tnet_packet_t* tnet_alloc_for_read(uint16_t data_size);

tnet_err_t tnet_driver_open(uint8_t* mac_addr);
tnet_err_t tnet_driver_send(tnet_packet_t* packet);
tnet_err_t tnet_driver_read(tnet_packet_t** packet);

typedef enum _tnet_protocol_t {
	TNET_PROTOCOL_ARP = 0x0806, //ARP协议
	TNET_PROTOCOL_IP = 0x0800, //IP协议
}tnet_protocol_t;

/*
* IP地址
*/
typedef union _tipaddr_t {
	uint8_t array[TNET_IPV4_ADDR_SIZE];     // 以数据形式存储的ip
	uint32_t addr;                          // 32位的ip地址
}tipaddr_t;


#define TARP_ENTRY_FREE		        0       // ARP表项空闲
#define TARP_ENTRY_OK	            1       // ARP表项解析成功
#define TARP_ENTRY_RESOLVING	    2       // ARP表项正在解析
#define TARP_TIMER_PERIOD           1       // ARP扫描周期，1s足够

/**
 * ARP表项
 */
typedef struct _tarp_entry_t {
	tipaddr_t ipaddr;                       // ip地址
	uint8_t	macaddr[TNET_MAC_ADDR_SIZE];    // mac地址
	uint8_t	state;                          // 状态位
	uint16_t tmo;                           // 当前超时
	uint8_t	retry_cnt;                      // 当前重试次数
}tarp_entry_t;

void tarp_init(void);
tnet_err_t tarp_make_request(const tipaddr_t* ipaddr);
void tarp_in(tnet_packet_t * packet);

void tnet_init(void);
void tnet_poll(void);
void tarp_poll(void);


#endif // TNET_TINY_H

