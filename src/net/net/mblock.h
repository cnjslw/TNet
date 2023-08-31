#ifndef __MBLOCK_H__
#define __MBLOCK_H__

/*code here*/
#include "nlist.h"
#include "nlocker.h"

/**
 * @brief 存储管理块器
 */
typedef struct _mblock_t {
    nlist_t free_list; // 空闲的消息队列
    void* start; // 起始块
    nlocker_t locker; // 控制锁
    sys_sem_t alloc_sem; // 同步信号量
} mblock_t;

net_err_t mblock_init(mblock_t* mblock, void* mem, int blk_size, int cnt, nlocker_type_t share_type);
void* mblock_alloc(mblock_t* block, int ms);
int mblock_free_cnt(mblock_t* block);

void mblock_free(mblock_t* mblock, void* block);
void mblock_destroy(mblock_t* mblock);

#endif