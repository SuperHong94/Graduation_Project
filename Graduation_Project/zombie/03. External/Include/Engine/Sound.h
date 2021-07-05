#pragma once

#include <FMOD/fmod.h>
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_codec.h>
#include <FMOD/fmod_errors.h>


#ifdef _DEBUG
#pragma comment(lib, "FMOD/x64/fmodL64_vc.lib")
#else
#pragma comment(lib, "FMOD/x64/fmod64_vc.lib")
#endif


#include "Resource.h"

class CSound :
	public CResource
{
public:
	static FMOD::System* g_pFMOD;

private:
	FMOD::Sound* m_pSound;
	list<FMOD::Channel*>	m_listChannel;



public:
	// 0 (무한반복) 
	void Play(int _iRoopCount, bool _bOverlap, float volume);
	void Stop();
	void SetVolume(float v) {};

	void RemoveChannel(FMOD::Channel* _pTargetChannel);

public:
	virtual void Load(const wstring& _strFilePath);

public:
	CSound();
	virtual ~CSound();
};

//enum ESOUNDKIND
//{
//	MainSound = 0,
//	Perion,
//	STAGE2,
//	YOUDIE,
//	YOUWIN,
//	SD_END,   //이펙트 끝 
//};
//enum EFFOUNDKIND //이펙트 사운드
//{
//	jump = 0,
//	click = 1,
//	teleP,
//	Damage,
//	EFFSD_END
//};

//void soundSetup();
//void effSoundSetup();
//void playSound(ESOUNDKIND eSound);
//void effSoundoff();
//void effPlaySound(EFFOUNDKIND eSound);
//void SoundPause();

//#pragma once
//#ifndef _CSOUND_H_
//#define _CSOUND_H_
//
//
//#define SOUND_MAX 1.0f
//#define SOUND_MIN 0.0f
//#define SOUND_DEFAULT 0.5f
//#define SOUND_WEIGHT 0.1f
//
//class CBGM {
//private:
//	static FMOD_SYSTEM* g_sound_system;
//
//	FMOD_SOUND* m_sound;
//	FMOD_CHANNEL* m_channel;
//
//	float m_volume;
//	FMOD_BOOL m_bool;
//public:
//	CBGM(const char* path, bool loop);
//	~CBGM();
//
//	static int Init();
//	static int Release();
//
//	int play();
//	int pause();
//	int resume();
//	int stop();
//	int volumeUp();
//	int volumeDown();
//
//	int Update();
//};
//
//#endif;