#include "tnet_tiny.h"
#include <string.h>

#define min(a,b) ((a) > (b) ? (b) : (a))

static const tipaddr_t netif_ipaddr = TNET_CFG_NETIF_IP;
static const uint8_t ether_broadcast[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
static tnet_packet_t tx_packet, rx_packet;//用于收发数据的包
static uint8_t netif_mac[TNET_MAC_ADDR_SIZE];//mac地址
static tarp_entry_t arp_entry;
static tnet_time_t arp_timer;

#define swap_order16(v)   ((((v) & 0xFF) << 8) | (((v) >> 8) & 0xFF))
#define tipaddr_is_equal_buf(addr, buf)      (memcmp((addr)->array, (buf), TNET_IPV4_ADDR_SIZE) == 0)


/*
* 检查超时情况
*/
int tnet_check_tmo(tnet_time_t * time , uint32_t sec)
{
	tnet_time_t curr = tsys_get_time();
	if (sec == 0)
	{
		*time = curr;
		return 0;
	}
	else if ((curr - *time) >= sec)
	{
		*time = curr;
		return 1;
	}
	return 0;
}

/*
* 分配一个网络数据包用户发送数据
*/
tnet_packet_t* tnet_alloc_for_send(uint16_t data_size)
{
	tx_packet.data = tx_packet.payload + TNET_CFG_PACKET_MAX_SIZE- data_size;
	tx_packet.size = data_size;
	return &tx_packet;
}

/*
* 分配一个网络数据包用户读取数据
*/
tnet_packet_t* tnet_alloc_for_read(uint16_t data_size)
{
	rx_packet.data = rx_packet.payload;
	rx_packet.size = data_size;
	return &rx_packet;
}

/*
* 添加一个头部
*/
static void add_header(tnet_packet_t * packet , uint16_t header_size)
{
	packet->data -= header_size;
	packet->size += header_size;
}

/*
* 移除一个头部
*/
static void remove_header(tnet_packet_t* packet, uint16_t header_size)
{
	packet->data += header_size;
	packet->size -= header_size;
}

/*
* 将包的长度截断为size
*/
static void truncate_packet(tnet_packet_t* packet, uint16_t size)
{
	
	packet->size = min(packet->size , size);
}


/*
 * 以太网初始化
 */
static tnet_err_t ethernet_init(void)
{
	tnet_err_t err = tnet_driver_open(netif_mac);
	if (err < 0)return err;
	return tarp_make_request(&netif_ipaddr);//以太网初始化就发送一个ARP包
	//return TNET_ERR_OK;
}

/*
*发送一个以太网数据帧
*/
static tnet_err_t ethernet_out_to(tnet_protocol_t protocol, const uint8_t* mac_addr, tnet_packet_t* packet)
{
	tether_hdr_t* ether_hdr;
	//添加头
	add_header(packet , sizeof(tether_hdr_t));
	ether_hdr = (tether_hdr_t*)packet->data;
	memcpy(ether_hdr->dest , mac_addr , TNET_MAC_ADDR_SIZE);
	memcpy(ether_hdr->src , netif_mac , TNET_MAC_ADDR_SIZE);
	ether_hdr->protocol = swap_order16(protocol);
	//发送数据
	return tnet_driver_send(packet);
}


/**
 * 以太网数据帧输入输出
 */
static void ethernet_in(tnet_packet_t* packet) {
	// 至少要比头部数据大
	if (packet->size <= sizeof(tether_hdr_t)) {
		return;
	}

	// 往上分解到各个协议处理
	tether_hdr_t* hdr = (tether_hdr_t*)packet->data;
	switch (swap_order16(hdr->protocol)) {
	case TNET_PROTOCOL_ARP:
		remove_header(packet , sizeof(tether_hdr_t));
		tarp_in(packet);
		break;
	case TNET_PROTOCOL_IP: {
		break;
	}
	}
}

/**
 * 查询网络接口，看看是否有数据包，有则进行处理
 */
static void ethernet_poll(void) {
	tnet_packet_t* packet;

	if (tnet_driver_read(&packet) == TNET_ERR_OK) {
		ethernet_in(packet);
	}
}

/**
 * ARP初始化
 */
void tarp_init(void) {
	arp_entry.state = TARP_ENTRY_FREE;
	//获取当前时间
	tnet_check_tmo(&arp_timer , 0);
}

/*
* 查询ARP表项是否超时,超时则重新请求
*/
void tarp_poll(void)
{
	if (tnet_check_tmo(&arp_timer, TARP_TIMER_PERIOD))
	{
		switch (arp_entry.state)
		{
		case TARP_ENTRY_RESOLVING : 
			if (--arp_entry.tmo == 0) {
				if (arp_entry.retry_cnt-- == 0)
				{
					arp_entry.state = TARP_ENTRY_FREE;
				}
				else
				{//继续重新探测
					tarp_make_request(&arp_entry.ipaddr);
					arp_entry.state = TARP_ENTRY_RESOLVING;
					arp_entry.tmo = TARP_CFG_ENTRY_PENDING_TMO;
				}
			}
			break;
		case TARP_ENTRY_OK:
			if (--arp_entry.tmo == 0)
			{
				tarp_make_request(&arp_entry.ipaddr);
				arp_entry.state = TARP_ENTRY_RESOLVING;
				arp_entry.tmo = TARP_CFG_ENTRY_PENDING_TMO;
			}
			break;
		}
	}
}

/**
* 生成一个ARP响应
*/
tnet_err_t tarp_make_response(tarp_packet_t* arp_packet)
{
	tarp_packet_t* response_packet;
	tnet_packet_t* packet = tnet_alloc_for_send(sizeof(tarp_packet_t));

	response_packet = (tarp_packet_t*)packet->data;
	response_packet->hw_type = swap_order16(TARP_HW_ETHER);
	response_packet->pro_type = swap_order16(TNET_PROTOCOL_IP);
	response_packet->hw_len = TNET_MAC_ADDR_SIZE;
	response_packet->pro_len = TNET_IPV4_ADDR_SIZE;
	response_packet->opcode = swap_order16(TARP_REQUEST);
	memcpy(response_packet->target_mac, arp_packet->sender_mac, TNET_MAC_ADDR_SIZE);
	memcpy(response_packet->target_ip, arp_packet->sender_ip, TNET_IPV4_ADDR_SIZE);
	memcpy(response_packet->sender_mac, netif_mac, TNET_MAC_ADDR_SIZE);
	memcpy(response_packet->sender_ip, netif_ipaddr.array, TNET_IPV4_ADDR_SIZE);
	return ethernet_out_to(TNET_PROTOCOL_ARP, ether_broadcast, packet);
}

/**
* ARP请求
* @para ipaddr 虚拟IP地址
*/
tnet_err_t tarp_make_request(const tipaddr_t* ipaddr)
{
	tarp_packet_t* arp_packet;
	tnet_packet_t* packet = tnet_alloc_for_send(sizeof(tarp_packet_t));

	arp_packet = (tarp_packet_t*)packet->data;
	arp_packet->hw_type = swap_order16(TARP_HW_ETHER);
	arp_packet->pro_type = swap_order16(TNET_PROTOCOL_IP);
	arp_packet->hw_len = TNET_MAC_ADDR_SIZE;
	arp_packet->pro_len = TNET_IPV4_ADDR_SIZE;
	arp_packet->opcode = swap_order16(TARP_REQUEST);
	memcpy(arp_packet->sender_mac, netif_mac, TNET_MAC_ADDR_SIZE);
	memcpy(arp_packet->sender_ip, netif_ipaddr.array, TNET_IPV4_ADDR_SIZE);
	memset(arp_packet->target_mac, 0, TNET_MAC_ADDR_SIZE);
	memcpy(arp_packet->target_ip, ipaddr->array, TNET_IPV4_ADDR_SIZE);
	return ethernet_out_to(TNET_PROTOCOL_ARP, ether_broadcast, packet);
}


/*
* 更新ARP表
*/
static void update_arp_entry(uint8_t * src_ip , uint8_t * mac_addr)
{
	memcpy(arp_entry.ipaddr.array , src_ip ,TNET_IPV4_ADDR_SIZE);
	memcpy(arp_entry.macaddr , mac_addr , 6);
	arp_entry.state = TARP_ENTRY_OK;
	arp_entry.tmo = TARP_CFG_ENTRY_OK_TMO;
	arp_entry.retry_cnt = TARP_CFG_MAX_RETRIES;
}


/*
* ARP输入处理
*/
void tarp_in(tnet_packet_t* packet)
{
	if (packet->size >= sizeof(tarp_packet_t))
	{
		tarp_packet_t* arp_packet = (tarp_packet_t*)packet->data;
		uint16_t opcode = swap_order16(arp_packet->opcode);

		//包的合法性检查
		if ((swap_order16(arp_packet->hw_type) != TARP_HW_ETHER) ||
			(arp_packet->hw_len != TNET_MAC_ADDR_SIZE) ||
			(swap_order16(arp_packet->pro_type) != TNET_PROTOCOL_IP) ||
			(arp_packet->pro_len != TNET_IPV4_ADDR_SIZE)
			|| ((opcode != TARP_REQUEST) && (opcode != TARP_REPLY))) {
			return;
		}

		//只处理发给自己的请求包和响应包
		if (!tipaddr_is_equal_buf(&netif_ipaddr, arp_packet->target_ip))return;

		//根据操作码进行不同的处理
		switch (swap_order16(arp_packet->opcode)) {
		case TARP_REQUEST:
			tarp_make_request(arp_packet);
			update_arp_entry(arp_packet->sender_ip , arp_packet->sender_mac);
			break;
		case TARP_REPLY:
			update_arp_entry(arp_packet->sender_ip , arp_packet->sender_mac);
			break;
		}
	}
}

/**
 * 协议栈的初始化
 */
void tnet_init(void) {
	ethernet_init();
	tarp_init();
}

/**
 * 轮询处理数据包，并在协议栈中处理
 */
void tnet_poll(void) {
	ethernet_poll();
	tarp_poll();
}
