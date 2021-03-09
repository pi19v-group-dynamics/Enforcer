#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdatomic.h>

typedef atomic_flag spinlock_t;

#define spinlock_lock()
#define spinlock_unlock()

#endif /* SPINLOCK_H */

