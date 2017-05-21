#include "Music.h"
#include "QEngine.h"
#include "IQSound.h"

Music::Music(QEngine* eng)
{
	engine = eng;
	MusicHandle = -1;
	MusicChannel = -1;
	Playing = false;
	Paused = false;
	Volume = 0.3f;
}

int Music::Play()
{
	if (Playing) return 0;

	if (MusicHandle >= 0)
	{
		MusicChannel = engine->sound->PlaySound(MusicHandle, true);
		if(MusicChannel == -1) return 0;

		engine->sound->SetChannelVolume(MusicChannel, Volume);
		engine->sound->PauseChannel(MusicChannel, Paused);

		Playing = true;
		return 1;
	}
	else
	{
		engine->MsgX(L"No track loaded.", 0.04f, 10.0f);
		return -1;
	}
}

int Music::Stop()
{	
	//check if music is loaded
	if (MusicHandle == -1) return -1;

	if (Playing)
	{
		engine->sound->StopChannel(MusicChannel);

		Playing = false;
		Paused = false;
		return 1;
	}
	else return 0;
}

int Music::SetPaused(bool paused)
{
	if(Playing && (MusicChannel >= 0))
	{
		int result = engine->sound->PauseChannel(MusicChannel, paused);
		Paused = paused;

		return result;
	}
	else return -1;
}

int Music::Load(const char *FileName)
{
	bool play = Playing;
	Stop();
	MusicHandle = engine->sound->Create2DStream(FileName);
	if (MusicHandle == -1)
	{
		engine->Msg(L"Couldn't load track.");
		return -1;
	}

	if(play) Play();
	return 1;
}

int Music::SetVolume(float vol)
{
	Volume = vol;
	if(Playing)
	{
		engine->sound->SetChannelVolume(MusicChannel, Volume);
		return 1;
	}
	else return 0;
}

void Music::SwitchPaused()
{
	if(Paused) SetPaused(false);
	else SetPaused(true);
}