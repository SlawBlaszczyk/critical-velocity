#ifndef QSOUNDFMOD_H
#define QSOUNDFMOD_H

#include "..\QEngine2 Proto1\IQSound.h"
#include "..\QEngine2 Proto1\Structures.h"
#include "fmod.hpp"

#define MAX_SOUNDS 256
#define MAX_DSP 256
#define MAX_CHANNELS 100

using namespace FMOD;

class QSoundFMOD : public IQSound
{
protected:
	HWND hWnd;
	HRESULT hResult;
	FMOD_RESULT result;
	unsigned int version;

	//FMOD main components
	System* system;
	Sound* sounds [MAX_SOUNDS];
	DSP* dsps [MAX_DSP];
	Channel* channel;

	FMOD_VECTOR listenerpos;
	FMOD_SPEAKERMODE speakermode;


public:
	QSoundFMOD (HINSTANCE hDLL_);
	~QSoundFMOD();

	//config window message handler procedure
	BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

	//interface functions:
	HRESULT Init(HWND parent);

	//updates fmod system status, to be called each frame
	void Update();

	//updates listener position
	void UpdateListener(int index, QVector3 position, QVector3 velocity, QVector3 fwd, QVector3 up);

	//creates a 2D sample object and returns its handle
	int Create2DSample(const char* FileName, bool loop = false);

	//creates a 3D sample object and returns its handle
	int Create3DSample(const char* FileName, bool loop = false);

	//creates a 2D stream object and returns its handle
	int Create2DStream(const char* FileName);

	//creates a 3D stream object and returns its handle
	int Create3DStream(const char* FileName);

	//creates a dsp and returns handle
	int CreateDSPEffect(int type);

	//plays a dsp effect and returns channel handle
	int PlayDSPEffect(int hDSP, bool paused);

	//deallocates a stream or a sample from memory goven its handle
	int DestroySound(int hSound);

	//plays sample or a stream and returns a channel handle
	int PlaySound(int hSound, bool paused = false);

	//plays sample or a stream at a given volume and returns a channel handle
	int PlaySound(int hSound, float volume, bool paused = false);

	//pauses or unpauses a channel given its handle
	int PauseChannel(int hChannel, bool pause);

	//stops a channel given its handle
	int StopChannel(int hChannel);

	//sets the volume of a channel
	int SetChannelVolume(int hChannel, float vol);

	//sets the position of a 3D channel
	int Set3DChannelParams(int hChannel, QVector3 position, QVector3 velocity);

	//adds a dsp unit to channel
	int AddChannelDSP(int hChannel, int hDSP);

	void GetChannelFrequency(int chan, float* freq);
	void SetChannelFrequency(int chan, float freq);
};

#endif