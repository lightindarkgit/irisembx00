#ifndef _AUDIOPLAY_H_
#define _AUDIOPLAY_H_

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include <QtCore/QCoreApplication>


#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include <unistd.h>
#include <sys/time.h>

#include <iostream>
#include <fstream>

#define SVCN  "com.irisking.audio.play"

typedef enum AudioID
{
    //靠近，靠远
    CLOSER = 1,
    FARTHER,

    ENROLLSUCC,
    IDENSUCC,

    RETRY,
    ENROLLSTOP,

    SLOW,
    KEEP,
    UP,
    DOWN,
    LEFT,
    RIGHT,

    CAMERA,

    THANKS,
    WELCOME,

    END, //kill daemon

} AudioID;

class AudioPlay : public QObject
{
    Q_OBJECT

    public:
        AudioPlay();
        ~AudioPlay();

    void PlaySound(AudioID aID);
    int OSVersion(void);

    public slots:
    Q_SCRIPTABLE int Play(int aID);

    private:

    AudioPlay(const AudioPlay&);
    AudioPlay& operator=(const AudioPlay&);
    int os;

    private:
        Mix_Chunk *pCLOSER;
        Mix_Chunk *pFARTHER;
        Mix_Chunk *pENROLLSUCC;
        Mix_Chunk *pENROLLSTOP;
        Mix_Chunk *pIDENSUCC;
        Mix_Chunk *pRETRY;
        Mix_Chunk *pSLOW;
        Mix_Chunk *pCAMERA;
        Mix_Chunk *pKEEP;
        Mix_Chunk *pUP;
        Mix_Chunk *pDOWN;
        Mix_Chunk *pLEFT;
        Mix_Chunk *pRIGHT;
        Mix_Chunk *pTHANKS;
        Mix_Chunk *pWELCOME;
}; 

#endif
