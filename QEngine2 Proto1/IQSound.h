#ifndef IQSOUND_H
#define IQSOUND_H

#include <Windows.h>
#include "Structures.h"
#include "QVector3.h"

#define DSP_TYPE_HISS 0

class QEngine;

class IQSound
{
public:

	HINSTANCE hDLL;

	//interface functions:

	//initializes sound subsystem
	virtual HRESULT Init(HWND parent) = 0;

	//updates fmod system status, to be called each frame
	virtual void Update() = 0;

	//updates listener position
	virtual void UpdateListener(int index, QVector3 position, QVector3 velocity, QVector3 fwd, QVector3 up) = 0;

	//creates a 2D sample object and returns its handle
	virtual int Create2DSample(const char* FileName, bool loop = false) = 0;

	//creates a 3D sample object and returns its handle
	virtual int Create3DSample(const char* FileName, bool loop = false) = 0;

	//creates a 2D stream object and returns its handle
	virtual int Create2DStream(const char* FileName) = 0;

	//creates a 3D stream object and returns its handle
	virtual int Create3DStream(const char* FileName) = 0;
	
	//creates a dsp and returns handle
	virtual int CreateDSPEffect(int type) = 0;

	//plays a dsp effect and returns channel handle
	virtual int PlayDSPEffect(int hDSP, bool paused) = 0;

	//deallocates a stream or a sample from memory goven its handle
	virtual int DestroySound(int hSound) = 0;

	//plays sample or a stream and returns a channel handle
	virtual int PlaySound(int hSound, bool paused = false) = 0;
	
	//plays sample or a stream at a given volume and returns a channel handle
	virtual int PlaySound(int hSound, float volume, bool paused = false) = 0;

	//pauses or unpauses a channel given its handle
	virtual int PauseChannel(int hChannel, bool pause) = 0;

	//stops a channel given its handle
	virtual int StopChannel(int hChannel) = 0;

	//sets the volume of a channel
	virtual int SetChannelVolume(int hChannel, float vol) = 0;

	//sets the position of a 3D sound
	virtual int Set3DChannelParams(int hChannel, QVector3 position, QVector3 velocity) = 0;

	//adds a dsp unit to channel
	virtual int AddChannelDSP(int hChannel, int hDSP) = 0;

	virtual void GetChannelFrequency(int chan, float* freq) = 0;
	virtual void SetChannelFrequency(int chan, float freq) = 0;
};

extern "C"
{
	HRESULT CreateSoundSubsystem(HINSTANCE hDLL, IQSound** pInterface);
	typedef HRESULT (*CREATESOUNDSUBSYSTEM) (HINSTANCE hDLL, IQSound** pInterface);

	HRESULT ReleaseSoundSubsystem(IQSound** pInterface);
	typedef HRESULT (*RELEASESOUNDSUBSYSTEM) (IQSound** pInterface);
}

#endif