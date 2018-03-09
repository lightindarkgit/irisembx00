/*****************************************************************************
 *  功    能：声音播放类
 *  说    明：此方法先把音频文件加载到内存，播放时直接通过Mix播放。
 *  	      播一次大概十几微秒，如果有阻塞以后可优化，启线程播放。
 *  依赖    ：需要安装 libsdl1.2debian libsdl-mixer1.2 这两库
 *  创 建 人：caotao
 *  创建时间：2016-11-24
 *****************************************************************************/

#include "AudioPlay.h"

unsigned long long getSystemTimeUs() {

    struct timeval us;

    gettimeofday(&us, NULL);

    return 1000000 * us.tv_sec + us.tv_usec;

}

int AudioPlay::OSVersion(void)
{
        int ret = -1;

        char line[50] = {'\0'};

        std::ifstream ifs("/etc/issue", std::ios::in);

        ifs.getline(line, sizeof(line));

        if(!strncmp(line, "Ubuntu 14.04.2 LTS", 18))
        {
                ret = 1;
        }
        else if(!strncmp(line, "Ubuntu 12.04.1 LTS", 18))
        {
                ret = 0;
        }

        printf("os /etc/issue: %s ret: %d\n", line, ret);

        ifs.clear();

        ifs.close();

        return ret;
}

AudioPlay::AudioPlay()
{
    SDL_Init(SDL_INIT_AUDIO);

    if(Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096) == -1)
    {
        std::cout << "Mix_OpenAudio Unable to open audio!\n" << std::endl;
    }

    //load wav....

    pCLOSER = Mix_LoadWAV("./sounds/chs/010closer.wav");
    if(NULL == pCLOSER)
    {
        std::cout << "load ./sounds/chs/010closer.wav faild" << std::endl;
    }

    pFARTHER = Mix_LoadWAV("./sounds/chs/018farther.wav");
    if(NULL == pFARTHER)
    {
        std::cout << "load ./sounds/chs/018farther.wav faild" << std::endl;
    }

    pENROLLSUCC = Mix_LoadWAV("./sounds/chs/003enrollsucc.wav");
    if(NULL == pENROLLSUCC)
    {
        std::cout << "load ./sounds/chs/003enrollsucc.wav failed!" << std::endl;
    }
    pENROLLSTOP = Mix_LoadWAV("./sounds/chs/006enrollstop.wav");
    if(NULL == pENROLLSTOP)
    {
        std::cout << "load ./sounds/chs/006enrollstop.wav failed!" << std::endl;
    }
    pIDENSUCC = Mix_LoadWAV("./sounds/chs/002idensucc.wav");
    if(NULL == pIDENSUCC)
    {
        std::cout << "load ./sounds/chs/002idensucc.wav failed!" << std::endl;
    }
    pRETRY = Mix_LoadWAV("./sounds/chs/005retry.wav");
    if(NULL == pRETRY)
    {
        std::cout << "load ./sounds/chs/005retry.wav failed!" << std::endl;
    }
    pSLOW = Mix_LoadWAV("./sounds/chs/107slow.wav");
    if(NULL == pSLOW)
    {
        std::cout << "load ./sounds/chs/107slow.wav failed!" << std::endl;
    }
    pCAMERA = Mix_LoadWAV("./sounds/chs/015-camera.wav");
    if(NULL == pCAMERA)
    {
        std::cout << "load ./sounds/chs/015-camera.wav failed!" << std::endl;
    }
    pKEEP = Mix_LoadWAV("./sounds/chs/100keep.wav");
    if(NULL == pKEEP)
    {
        std::cout << "load ./sounds/chs/100keep.wav failed!" << std::endl;
    }
    pUP = Mix_LoadWAV("./sounds/chs/103up.wav");
    if(NULL == pUP)
    {
        std::cout << "load ./sounds/chs/103up.wav failed!" << std::endl;
    }
    pDOWN = Mix_LoadWAV("./sounds/chs/102down.wav");
    if(NULL == pDOWN)
    {
        std::cout << "load ./sounds/chs/102down.wav failed!" << std::endl;
    }
    pLEFT = Mix_LoadWAV("./sounds/chs/105left.wav");
    if(NULL == pLEFT)
    {
        std::cout << "load ./sounds/chs/105left.wav failed!" << std::endl;
    }
    pRIGHT = Mix_LoadWAV("./sounds/chs/104right.wav");
    if(NULL == pRIGHT)
    {
        std::cout << "load ./sounds/chs/104right.wav failed!" << std::endl;
    }
    pTHANKS = Mix_LoadWAV("./sounds/chs/004thanks.wav");
    if(NULL == pTHANKS)
    {
        std::cout << "load ./sounds/chs/004thanks.wav failed!" << std::endl;
    }
    pWELCOME = Mix_LoadWAV("./sounds/chs/016welcome.wav");
    if(NULL == pWELCOME)
    {
        std::cout << "load ./sounds/chs/016welcome.wav failed!" << std::endl;
    }
    os = OSVersion();

}


AudioPlay::~AudioPlay()
{

    if(NULL != pCLOSER)
    {
        Mix_FreeChunk(pCLOSER);
    }

    if(NULL != pFARTHER)
    {
        Mix_FreeChunk(pFARTHER);
    }

    if(NULL != pENROLLSUCC)
    {
        Mix_FreeChunk(pENROLLSUCC);
    }
    if(NULL != pENROLLSTOP)
    {
        Mix_FreeChunk(pENROLLSTOP);
    }
    if(NULL != pIDENSUCC)
    {
        Mix_FreeChunk(pIDENSUCC);
    }
    if(NULL != pRETRY)
    {
        Mix_FreeChunk(pRETRY);
    }
    if(NULL != pSLOW)
    {
        Mix_FreeChunk(pSLOW);
    }
    if(NULL != pCAMERA)
    {
        Mix_FreeChunk(pCAMERA);
    }
    if(NULL != pKEEP)
    {
        Mix_FreeChunk(pKEEP);
    }
    if(NULL != pUP)
    {
        Mix_FreeChunk(pUP);
    }
    if(NULL != pDOWN)
    {
        Mix_FreeChunk(pDOWN);
    }
    if(NULL != pLEFT)
    {
        Mix_FreeChunk(pLEFT);
    }
    if(NULL != pRIGHT)
    {
        Mix_FreeChunk(pRIGHT);
    }
    if(NULL != pTHANKS)
    {
        Mix_FreeChunk(pTHANKS);
    }
    if(NULL != pWELCOME)
    {
        Mix_FreeChunk(pWELCOME);
    }

    Mix_CloseAudio();

    SDL_Quit();
}

void AudioPlay::PlaySound(AudioID aID)
{
#if 0
	Mix_HaltChannel(-1);

	printf("start PlaySound aID: %d\n", int(aID));
	switch(aID)
	{
		case CLOSER:
			Mix_PlayChannel( -1, pCLOSER, 0 );
			break;
		case FARTHER:
			Mix_PlayChannel( -1, pFARTHER, 0 );
			break;
		case ENROLLSUCC:
			Mix_PlayChannel( -1, pENROLLSUCC, 0 );
			break;
		case ENROLLSTOP:
			Mix_PlayChannel( -1, pENROLLSTOP, 0 );
			break;
		case IDENSUCC:
			Mix_PlayChannel( -1, pIDENSUCC, 0 );
			break;
		case RETRY:
			Mix_PlayChannel( -1, pRETRY, 0 );
			break;
		case SLOW:
			Mix_PlayChannel( -1, pSLOW, 0 );
			break;
		case CAMERA:
			Mix_PlayChannel( -1, pCAMERA, 0 );
			break;
		case KEEP:
			Mix_PlayChannel( -1, pKEEP, 0 );
			break;
		case UP:
			Mix_PlayChannel( -1, pUP, 0 );
			break;
		case DOWN:
			Mix_PlayChannel( -1, pDOWN, 0 );
			break;
		case LEFT:
			Mix_PlayChannel( -1, pLEFT, 0 );
			break;
		case RIGHT:
			Mix_PlayChannel( -1, pRIGHT, 0 );
			break;
		case THANKS:
			Mix_PlayChannel( -1, pTHANKS, 0 );
			break;
		case WELCOME:
			Mix_PlayChannel( -1, pWELCOME, 0 );
			break;
		default:
			break;
	}
	printf("end PlaySound aID: %d\n", int(aID));
#else
	if (!QDBusConnection::systemBus().isConnected())
	{
		fprintf(stderr, "Cannot connect to the D-Bus session bus.\n");
		//return 1;
	}
	if(1 == os)
	{

		QDBusInterface iface(SVCN, "/", "local.APS.AudioPlay", QDBusConnection::systemBus());

		if (iface.isValid()) 
		{

			unsigned long long t1 = getSystemTimeUs();
			QDBusReply<int> reply = iface.call("Play", int(aID));
			unsigned long long t2 = getSystemTimeUs();

			if (reply.isValid())
			{
				fprintf(stdout, "aps exectime: %lluus Reply was: %d\n", t2 - t1, reply.value());
				//return 0;
			}
			else
			{

				fprintf(stderr, "Call failed: %s\n", qPrintable(reply.error().message()));
			}
			//return 1;
		}
		else
		{

			fprintf(stderr, "%s\n", qPrintable(QDBusConnection::systemBus().lastError().message()));
		}
	}
	else
	{
		QDBusInterface iface(SVCN, "/", "", QDBusConnection::systemBus());

		if (iface.isValid()) 
		{

			unsigned long long t1 = getSystemTimeUs();
			QDBusReply<int> reply = iface.call("Play", int(aID));
			unsigned long long t2 = getSystemTimeUs();

			if (reply.isValid())
			{
				fprintf(stdout, "aps exectime: %lluus Reply was: %d\n", t2 - t1, reply.value());
				//return 0;
			}
			else
			{

				fprintf(stderr, "Call failed: %s\n", qPrintable(reply.error().message()));
			}
			//return 1;
		}
		else
		{

			fprintf(stderr, "%s\n", qPrintable(QDBusConnection::systemBus().lastError().message()));
		}
	}

#endif
}

int AudioPlay::Play(int aID)
{
        AudioID iaID = (AudioID)aID;

	Mix_HaltChannel(-1);

	fprintf(stdout, "start PlaySound aID: %d\n", int(aID));

	switch(iaID)
	{
		case CLOSER:
			Mix_PlayChannel( -1, pCLOSER, 0 );
			break;
		case FARTHER:
			Mix_PlayChannel( -1, pFARTHER, 0 );
			break;
		case ENROLLSUCC:
			Mix_PlayChannel( -1, pENROLLSUCC, 0 );
			break;
		case ENROLLSTOP:
			Mix_PlayChannel( -1, pENROLLSTOP, 0 );
			break;
		case IDENSUCC:
			Mix_PlayChannel( -1, pIDENSUCC, 0 );
			break;
		case RETRY:
			Mix_PlayChannel( -1, pRETRY, 0 );
			break;
		case SLOW:
			Mix_PlayChannel( -1, pSLOW, 0 );
			break;
		case CAMERA:
			Mix_PlayChannel( -1, pCAMERA, 0 );
			break;
		case KEEP:
			Mix_PlayChannel( -1, pKEEP, 0 );
			break;
		case UP:
			Mix_PlayChannel( -1, pUP, 0 );
			break;
		case DOWN:
			Mix_PlayChannel( -1, pDOWN, 0 );
			break;
		case LEFT:
			Mix_PlayChannel( -1, pLEFT, 0 );
			break;
		case RIGHT:
			Mix_PlayChannel( -1, pRIGHT, 0 );
			break;
		case THANKS:
			Mix_PlayChannel( -1, pTHANKS, 0 );
			break;
		case WELCOME:
			Mix_PlayChannel( -1, pWELCOME, 0 );
			break;
		default:
			break;
	}
	fprintf(stdout, "end PlaySound aID: %d\n", int(aID));
	return 0;
}
