#ifndef __NLOCKER_H__
#define __NLOCKER_H__

#include "net_err.h"
#include "sys_plat.h"
/**
 * @brief 锁类型
 */
typedef enum _nlocker_type_t {
    NLOCKER_NONE, // 不需要锁
    NLOCKER_THREAD, // 用于线程共享资源
} nlocker_type_t;

/**
 * @brief 通用锁结构
 */
typedef struct _nlocker_t {
    nlocker_type_t type; // 锁类型
    union {
        sys_mutex_t mutex;
    };
} nlocker_t;

net_err_t nlocker_init(nlocker_t* locker, nlocker_type_t type);
void nlocker_destroy(nlocker_t* locker);
void nlocker_lock(nlocker_t* locker);
void nlocker_unlock(nlocker_t* locker);

#endif