#include "SyncLock.h"
#include "../Common/Exectime.h"


SyncLock::SyncLock(IRISK_MUTEX* mutex)
{
	Exectime etm(__FUNCTION__);
	_pmutex = mutex;
#ifndef WIN32
	pthread_mutex_lock(_pmutex);
#else

#endif
}

SyncLock::~SyncLock()
{
	Exectime etm(__FUNCTION__);
#ifndef WIN32
	pthread_mutex_unlock(_pmutex);
#else

#endif
}

void SyncLock::Init(IRISK_MUTEX* mutex)
{
	Exectime etm(__FUNCTION__);
#ifndef WIN32
	pthread_mutex_init(&(*mutex),NULL);
#else

#endif
}

void SyncLock::Destroy(IRISK_MUTEX* mutex)
{
	Exectime etm(__FUNCTION__);
#ifndef WIN32
	pthread_mutex_destroy(&(*mutex));
#else

#endif
}

