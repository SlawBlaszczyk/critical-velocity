#ifndef MUSIC_H
#define MUSIC_H

class QEngine;

class Music
{
	QEngine* engine;

	int MusicHandle; //handle for played bgm
	int MusicChannel; //bgm channel
	bool Playing; //flag if music is started
	bool Paused;
	float Volume;

public:
	Music(QEngine* eng);
	int Play();
	int Stop();
	int SetPaused(bool paused);
	int Load(const char* FileName);
	int SetVolume (float vol);
	float GetVolume() {return Volume;}
	void SwitchPaused();

	bool GetPlaying() {return Playing;}
	bool GetPaused() {return Paused;}
};

#endif