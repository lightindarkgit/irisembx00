#ifndef	_SYNC_LOCK_H_
#define _SYNC_LOCK_H_

#include "Headers.h"

#ifndef WIN32
#define	IRISK_MUTEX		pthread_mutex_t
#else
#define	IRISK_MUTEX		pthread_mutex_t
#endif

class SyncLock
{
public:
	SyncLock(IRISK_MUTEX* mutex);
	~SyncLock();

	static void Init(IRISK_MUTEX* mutex);
	static void Destroy(IRISK_MUTEX* mutex);

private:
	IRISK_MUTEX*	_pmutex;
};

#endif

