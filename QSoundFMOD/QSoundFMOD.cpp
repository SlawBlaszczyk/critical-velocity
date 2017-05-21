#include <Windows.h>
#include "QSoundFMOD.h"

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

//an exported function that creates an instance of the subsystem 
//and returns a pInterface pointer of the subsystem to the main appliccation
HRESULT CreateSoundSubsystem(HINSTANCE hDLL, IQSound** pInterface)
{
	if (!*pInterface)
	{
		*pInterface = new QSoundFMOD(hDLL);
		return S_OK;
	}
	return E_FAIL;
}

//an exported function that releases the sound subsystem
HRESULT ReleaseSoundSubsystem(IQSound** pInterface)
{
	if (!*pInterface)
	{
		return E_FAIL;
	}
	delete *pInterface;
	*pInterface = NULL;
	return S_OK;
}

void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        MessageBox(0, L"Sound error", L"Sound error encountered", MB_OK);
    }
}

QSoundFMOD::QSoundFMOD(HINSTANCE hDLL_)
{
	hDLL = hDLL_;

	//nullify pointers
	system = NULL;
	channel = NULL;

	for(int i=0; i<MAX_SOUNDS; i++)
		sounds[i] = NULL;

	for(int i=0; i<MAX_DSP; i++)
		dsps[i] = NULL;
}

QSoundFMOD::~QSoundFMOD()
{
	//destroy objects
	for(int i=0; i<MAX_SOUNDS; i++)
		if (sounds[i] != NULL)
			sounds[i]->release();

	for(int i=0; i<MAX_DSP; i++)
		if (dsps[i] != NULL)
			dsps[i]->release();


	if (system != NULL)
    {
		system->close();
		system->release();
	}

}

HRESULT QSoundFMOD::Init(HWND parent)
{
	hWnd = parent;
	//MessageBox(parent, L"Sound Dziala", L"Message", MB_OK);

	//create system
	result = System_Create(&system);
    ERRCHECK(result);

	result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
		MessageBox(parent, L"Error!  You are using an old version of FMOD.", L"Error", MB_OK);
        return -1;
    }

	result = system->init(MAX_CHANNELS, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result);

	///////////////
	result = system->set3DSettings(1.0f, 10.0f, 1.0f);
    ERRCHECK(result);

	return hResult;
}

void QSoundFMOD::Update()
{
	if (system != NULL)
    {
		system->update();
	}
}

int QSoundFMOD::Create2DSample(const char* FileName, bool loop)
{
	//check for a free sound index
	for (int i=1; i<MAX_SOUNDS; i++)
	{
		if(sounds[i] == NULL) //allocate sound and return
		{
			if (loop)
			{
				result = system->createSound(FileName, FMOD_CREATESAMPLE | FMOD_LOOP_NORMAL, 0, &sounds[i]);
				ERRCHECK(result);
			}
			else
			{
				result = system->createSound(FileName, FMOD_CREATESAMPLE | FMOD_LOOP_NORMAL, 0, &sounds[i]);
				ERRCHECK(result);
			}

			if (result == FMOD_OK)
				return i;

			else return -1;
		}
	}

	//no sound slot available
	return -1;
}

int QSoundFMOD::Create3DSample(const char* FileName, bool loop)
{
	//check for a free sound index
	for (int i=1; i<MAX_SOUNDS; i++)
	{
		if(sounds[i] == NULL) //allocate sound and return
		{
			if (loop)
			{
				result = system->createSound(FileName, FMOD_CREATESAMPLE | FMOD_3D | FMOD_LOOP_NORMAL, 0, &sounds[i]);
				ERRCHECK(result);
			}
			else
			{
				result = system->createSound(FileName, FMOD_CREATESAMPLE | FMOD_3D , 0, &sounds[i]);
				ERRCHECK(result);
			}

			if (result == FMOD_OK)
				return i;

			else return -1;
		}
	}

	//no sound slot available
	return -1;
}

int QSoundFMOD::Create2DStream(const char* FileName)
{
	//check for a free sound index
	for (int i=1; i<MAX_SOUNDS; i++)
	{
		if(sounds[i] == NULL) //allocate sound and return
		{
			result = system->createSound(FileName, FMOD_CREATESTREAM | FMOD_2D |
				FMOD_LOOP_NORMAL , 0, &sounds[i]);
			ERRCHECK(result);

			if (result == FMOD_OK)
				return i;

			else return -1;
		}
	}

	//no sound slot available
	return -1;
}

int QSoundFMOD::Create3DStream(const char* FileName)
{
	//check for a free sound index
	for (int i=1; i<MAX_SOUNDS; i++)
	{
		if(sounds[i] == NULL) //allocate sound and return
		{
			result = system->createSound(FileName, FMOD_CREATESTREAM | FMOD_3D |
				FMOD_LOOP_NORMAL, 0, &sounds[i]);
			ERRCHECK(result);

			if (result == FMOD_OK)
				return i;

			else return -1;
		}
	}

	//no sound slot available
	return -1;
}

int QSoundFMOD::CreateDSPEffect(int type)
{
	//check for a free dsp index
	for (int i=1; i<MAX_DSP; i++)
	{
		if(dsps[i] == NULL) //allocate dsp and return
		{
			switch (type)
			{
			case DSP_TYPE_HISS:
				{
					//create 1000khz white noise
					result = system->createDSPByType(FMOD_DSP_TYPE_OSCILLATOR, &dsps[i]);
					ERRCHECK(result);

					result = dsps[i]->setParameter(FMOD_DSP_OSCILLATOR_RATE, 1000.0f);
					ERRCHECK(result);
					
					result = dsps[i]->setParameter(FMOD_DSP_OSCILLATOR_TYPE, 5);
                    ERRCHECK(result);

					if(result == FMOD_OK) return i;
					else return -1;
				}

			default:
				{
					//unknown type
					return -1;
				}
			}
		}
	}

	//no dsp slot available
	return -1;
}

int QSoundFMOD::PlayDSPEffect(int hDSP, bool paused)
{
	result = system->playDSP(FMOD_CHANNEL_FREE, dsps[hDSP], paused, &channel);
	ERRCHECK(result);

	//return channel index
	int index;
	result = channel->getIndex(&index);
	ERRCHECK(result);

	if(result == FMOD_OK) return index;
	else return -1;
}

int QSoundFMOD::DestroySound(int hSound)
{
	if (sounds[hSound] != NULL)
	{
		sounds[hSound]->release();
		return 1;
	}

	else return -1;
}

int QSoundFMOD::PlaySound(int hSound, bool paused)
{
	result = system->playSound(FMOD_CHANNEL_FREE, sounds[hSound], paused, &channel);
	ERRCHECK(result);

	//return channel index
	int index;
	result = channel->getIndex(&index);
	ERRCHECK(result);

	if(result == FMOD_OK) return index;
	else return -1;
}

int QSoundFMOD::PlaySound(int hSound, float volume, bool paused)
{
	result = system->playSound(FMOD_CHANNEL_FREE, sounds[hSound], paused, &channel);
	ERRCHECK(result);

	result = channel->setVolume(volume);

	//return channel index
	int index;
	result = channel->getIndex(&index);
	ERRCHECK(result);

	if(result == FMOD_OK) return index;
	else return -1;
}

int QSoundFMOD::PauseChannel(int hChannel, bool pause)
{
	//get channel from index
	result = system->getChannel(hChannel, &channel);
	ERRCHECK(result);

	result = channel->setPaused(pause);
	
	if(result == FMOD_OK) return 1;
	else if (result == FMOD_ERR_INVALID_HANDLE) return 0; //channel no longer active
	else return -1;
}

int QSoundFMOD::StopChannel(int hChannel)
{
	//get channel from index
	result = system->getChannel(hChannel, &channel);
	ERRCHECK(result);

	result = channel->stop();
	
	if(result == FMOD_OK) return 1;
	else if (result == FMOD_ERR_INVALID_HANDLE) return 0; //channel no longer active
	else return -1;
}

int QSoundFMOD::SetChannelVolume(int hChannel, float vol)
{
	//get channel from index
	result = system->getChannel(hChannel, &channel);
	ERRCHECK(result);

	result = channel->setVolume(vol);
	
	if(result == FMOD_OK) return 1;
	else if (result == FMOD_ERR_INVALID_HANDLE) return 0; //channel no longer active
	else return -1;
}

void QSoundFMOD::UpdateListener(int index, QVector3 position, QVector3 velocity, QVector3 fwd, QVector3 up)
{
	FMOD_VECTOR pos = {position.x, position.y, position.z};
	FMOD_VECTOR vel = {velocity.x, velocity.y, velocity.z};
	FMOD_VECTOR forward = {fwd.x, fwd.y, fwd.z};
	FMOD_VECTOR upp = {up.x, up.y, up.z};
	
	result = system->set3DListenerAttributes(index, &pos, &vel, &forward, &upp);
	ERRCHECK(result);
}

int QSoundFMOD::Set3DChannelParams(int hChannel, QVector3 position, QVector3 velocity)
{
	FMOD_VECTOR pos = {position.x, position.y, position.z};
	FMOD_VECTOR vel = {velocity.x, velocity.y, velocity.z};

	//get channel from index
	result = system->getChannel(hChannel, &channel);
	ERRCHECK(result);

	result = channel->set3DAttributes(&pos, &vel);
	ERRCHECK(result);

	return 1;
}

int QSoundFMOD::AddChannelDSP(int hChannel, int hDSP)
{
	//get channel from index
	result = system->getChannel(hChannel, &channel);
	ERRCHECK(result);

	//result = channel->addDSP(dsps[hDSP]);
	//ERRCHECK(result);

	return 1;
}

void QSoundFMOD::GetChannelFrequency(int chan, float* freq)
{
	//get channel from index
	result = system->getChannel(chan, &channel);
	ERRCHECK(result);

	result = channel->getFrequency(freq);
	ERRCHECK(result);
}

void QSoundFMOD::SetChannelFrequency(int chan, float freq)
{
	//get channel from index
	result = system->getChannel(chan, &channel);
	ERRCHECK(result);

	result = channel->setFrequency(freq);
	ERRCHECK(result);
}