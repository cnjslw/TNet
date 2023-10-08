#ifndef XNET_TINY_H
#define XNET_TINY_H

#include <stdint.h>

#define XNET_CFG_NETIF_IP               {192, 168, 74, 2}  // 本机网卡IP
#define XNET_CFG_PACKET_MAX_SIZE        1516        // 收发数据包的最大大小
#define XARP_CFG_ENTRY_OK_TMO	        (5)         // ARP表项超时时间
#define XARP_CFG_ENTRY_PENDING_TMO	    (1)          // ARP表项挂起超时时间
#define XARP_CFG_MAX_RETRIES		    4                   // ARP表挂起时重试查询次数
#define XUDP_CFG_MAX_UDP                10                  //最大支持的UDP连接数

typedef enum _xnet_err_t {
    XNET_ERR_OK = 0,
    XNET_ERR_IO = -1,
    XNET_ERR_NONE = -2,
    XNET_ERR_BINDED = -3,
    XNET_ERR_PARAM = -4,
    XNET_ERR_MEM = -5,
}xnet_err_t;

typedef enum _xnet_protocol_t {
    XNET_PROTOCOL_ARP = 0x0806,     // ARP协议
    XNET_PROTOCOL_IP = 0x0800,      // IP协议
    XNET_PROTOCOL_ICMP = 1,         //ICMP协议
    XNET_PROTOCOL_UDP = 17,         // UDP协议

}xnet_protocol_t;

#define XNET_IPV4_ADDR_SIZE             4           // IP地址长度
#define XNET_MAC_ADDR_SIZE              6           // MAC地址长度

/*******************************各层协议数据头和通用数据包*******************************/

/**
 * 通用网络数据包
 */
typedef struct _xnet_packet_t {
    uint16_t size;                              // 包中有效数据大小
    uint8_t* data;                             // 包的数据起始地址
    uint8_t payload[XNET_CFG_PACKET_MAX_SIZE];  // 最大负载数据量
}xnet_packet_t;

typedef struct _xether_hdr_t xether_hdr_t;//以太网协议头
typedef struct _xarp_entry_t xarp_entry_t;//ARP协议头
typedef struct _xip_hdr_t xip_hdr_t;//IP协议头
typedef struct _xicmp_hdr_t xicmp_hdr_t;//ICMP协议头
typedef union _xipaddr_t xipaddr_t;//IP地址
typedef struct _xudp_t xudp_t; //UDP协议头



/*******************************网络数据包*******************************/

/*
* 网络数据的发送与读取
*/
xnet_packet_t* xnet_alloc_for_send(uint16_t data_size);
xnet_packet_t* xnet_alloc_for_read(uint16_t data_size);
void truncate_packet(xnet_packet_t* packet, uint16_t size);

typedef uint32_t xnet_time_t;           // 时间类型，返回当前系统跑了多少个100ms
const xnet_time_t xsys_get_time(void);
int xnet_check_tmo(xnet_time_t* time, uint32_t sec);

/*******************************UDP*******************************/

/*
* UDP块的回调函数
*/
typedef xnet_err_t(*xudp_handler_t)(xudp_t* udp, xipaddr_t* src_ip, uint16_t src_port, xnet_packet_t* packet);

/*
* UDP管理块
*/
struct _xudp_t {
    enum {
        XUDP_STATE_FREE,            // UDP未使用
        XUDP_STATE_USED,            // UDP已使用
    } state;                        // 状态

    uint16_t local_port;            // 本地端口
    xudp_handler_t handler;         // 事件处理回调
};

/*
* UDP数据头
*/
typedef struct _xudp_hdr_t {
    uint16_t src_port, dest_port;   // 源端口 + 目标端口
    uint16_t total_len;	            // 整个数据包的长度
    uint16_t checksum;		        // 校验和

}xudp_hdr_t;

/*
* UDP接口
*/
void xudp_init(void);
void xudp_in(xudp_t* udp, xipaddr_t* src_ip, xnet_packet_t* packet);
int xudp_out(xudp_t* udp, xipaddr_t* dest_ip, uint16_t dest_port, xnet_packet_t* packet);
xudp_t* xudp_open(xudp_handler_t handler);
void xudp_close(xudp_t* udp);
xudp_t* xudp_find(uint16_t port);
xnet_err_t xudp_bind(xudp_t* udp, uint16_t local_port);

/*******************************ICMP协议*******************************/

#define XICMP_CODE_ECHO_REQUEST             8           //回显请求
#define XICMP_CODE_ECHO_REPLY               0           //回显响应
#define XICMP_TYPE_UNREACH                  3           // 目的不可达
#define XICMP_CODE_PORT_UNREACH             3           // 端口不可达
#define XICMP_CODE_PRO_UNREACH              2           // 协议不可达

/*
* ICMP包头部
*/
typedef struct _xicmp_hdr_t {
    uint8_t type; //类型
    uint8_t code; //代码
    uint16_t checksum; //ICMP校验和
    uint16_t id; //标识符
    uint16_t seq; //序号
}xicmp_hdr_t;

/*
* ICMP接口
*/
void xicmp_init(void);
void xicmp_in(xipaddr_t * src_ip , xnet_packet_t * packet);
xnet_err_t xicmp_dest_unreach(uint8_t code , xip_hdr_t *ip_hdr);


/*******************************IP协议*******************************/

#define XNET_VERSION_IPV4                   4           // IPV4
#define XNET_IP_DEFAULT_TTL                 64         // 缺省的IP包TTL值

/*
* IP数据头
*/
#pragma pack(1)

typedef struct _xip_hdr_t {
    uint8_t hdr_len : 4;                // 首部长, 4字节为单位
    uint8_t version : 4;                // 版本号
    uint8_t tos;		                // 服务类型
    uint16_t total_len;		            // 总长度
    uint16_t id;		                // 标识符
    uint16_t flags_fragment;            // 标志与分段
    uint8_t ttl;                        // 存活时间
    uint8_t protocol;	                // 上层协议
    uint16_t hdr_checksum;              // 首部校验和
    uint8_t	src_ip[XNET_IPV4_ADDR_SIZE];        // 源IP
    uint8_t dest_ip[XNET_IPV4_ADDR_SIZE];	    // 目标IP
}xip_hdr_t;

#pragma pack()

/**
 * IP地址
 */
typedef union _xipaddr_t {
    uint8_t array[XNET_IPV4_ADDR_SIZE];     // 以数据形式存储的ip
    uint32_t addr;                          // 32位的ip地址
}xipaddr_t;


void xip_init(void);
void xip_in(xnet_packet_t* packet);
xnet_err_t xip_out(xnet_protocol_t protocol, xipaddr_t* dest_ip, xnet_packet_t* packet);

/*******************************ARP协议*******************************/

#define XARP_HW_ETHER               0x1         // 以太网
#define XARP_REQUEST                0x1         // ARP请求包
#define XARP_REPLY                  0x2         // ARP响应包


#define XARP_ENTRY_FREE		        0       // ARP表项空闲
#define XARP_ENTRY_OK		        1       // ARP表项解析成功
#define XARP_ENTRY_RESOLVING	    2       // ARP表项正在解析
#define XARP_TIMER_PERIOD           1       // ARP扫描周期，1s足够


/**
 * ARP表项
 */
typedef struct _xarp_entry_t {
    xipaddr_t ipaddr;                       // ip地址
    uint8_t	macaddr[XNET_MAC_ADDR_SIZE];    // mac地址
    uint8_t	state;                          // 状态位
    uint16_t tmo;                           // 当前超时
    uint8_t	retry_cnt;                      // 当前重试次数
}xarp_entry_t;

/**
 * ARP协议头
 */
typedef struct _xarp_packet_t {
    uint16_t hw_type, pro_type;                 // 硬件类型和协议类型
    uint8_t hw_len, pro_len;                    // 硬件地址长 + 协议地址长
    uint16_t opcode;                            // 请求/响应
    uint8_t sender_mac[XNET_MAC_ADDR_SIZE];     // 发送包硬件地址
    uint8_t sender_ip[XNET_IPV4_ADDR_SIZE];     // 发送包协议地址
    uint8_t target_mac[XNET_MAC_ADDR_SIZE];     // 接收方硬件地址
    uint8_t target_ip[XNET_IPV4_ADDR_SIZE];     // 接收方协议地址
}xarp_packet_t;


//ARP相关方法
void xarp_init(void);
xnet_err_t xarp_make_request(const xipaddr_t* ipaddr);
void xarp_in(xnet_packet_t* packet);
xnet_err_t xarp_resolve(const xipaddr_t* ipaddr, uint8_t** mac_addr);
void xarp_poll(void);


/*******************************以太网数据帧格式：RFC894*******************************/

//以太网协议头

#pragma pack(1)

typedef struct _xether_hdr_t {
    uint8_t dest[XNET_MAC_ADDR_SIZE];           // 目标mac地址
    uint8_t src[XNET_MAC_ADDR_SIZE];            // 源mac地址
    uint16_t protocol;                          // 协议/长度
}xether_hdr_t;

#pragma pack()

//与链路层的交互接口
xnet_err_t xnet_driver_open(uint8_t* mac_addr);
xnet_err_t xnet_driver_send(xnet_packet_t* packet);
xnet_err_t xnet_driver_read(xnet_packet_t** packet);


/*******************************全局初始化*******************************/

void xnet_init(void);
void xnet_poll(void);

#endif // XNET_TINY_H

