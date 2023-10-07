#ifndef __NET_CFG_H__
#define __NET_CFG_H__

/*code here*/
#define DBG_MBLOCK DBG_LEVEL_INFO
#define DBG_QUEUE DBG_LEVEL_INFO
#define DBG_MSG DBG_LEVEL_INFO
#define DBG_BUF DBG_LEVEL_INFO

#define NET_EXMSG_CNT 10 // 消息缓冲区大小

#define EXMSG_LOCKER NLOCKER_THREAD // 锁的类型

#define PKTBUF_BLK_SIZE 1024 // 数据包块大小
#define PKTBUF_BLK_CNT 2048 // 数据包块的数量
#define PKTBUF_BUF_CNT 1024 // 数据包的数量

#endif