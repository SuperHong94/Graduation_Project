#include "stdafx.h"
#include "Sound.h"

#include "ResMgr.h"


FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
	, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
	, void* commanddata1, void* commanddata2)
{
	FMOD::Channel* cppchannel = (FMOD::Channel*)channelcontrol;
	CSound* pSound = nullptr;

	switch (controltype)
	{
	case FMOD_CHANNELCONTROL_CALLBACK_END:
	{
		cppchannel->getUserData((void**)&pSound);
		pSound->RemoveChannel(cppchannel);
	}
	break;
	}

	return FMOD_OK;
}


FMOD::System* CSound::g_pFMOD = nullptr;

CSound::CSound()
	: CResource(RES_TYPE::SOUND)
	, m_pSound(nullptr)
{
}

CSound::~CSound()
{
	if (nullptr != m_pSound)
	{
		m_pSound->release();
		m_pSound = nullptr;
	}
}

void CSound::Play(int _iRoopCount, bool _bOverlap, float volume)
{
	if (_iRoopCount <= -1)
	{
		assert(nullptr);
	}

	// 중복 재생 허용 안할 경우
	if (!_bOverlap && !m_listChannel.empty())
	{
		return;
	}

	_iRoopCount -= 1;

	FMOD::Channel* pChannel = nullptr;
	g_pFMOD->playSound(m_pSound, nullptr, false, &pChannel);

	pChannel->setVolume(volume);

	pChannel->setCallback(CHANNEL_CALLBACK);
	pChannel->setUserData(this);

	pChannel->setMode(FMOD_LOOP_NORMAL);
	pChannel->setLoopCount(_iRoopCount);

	//Stop();
}

void CSound::Stop()
{
	list<FMOD::Channel*>::iterator iter;

	while (!m_listChannel.empty())
	{
		iter = m_listChannel.begin();
		(*iter)->stop();
	}
}

void CSound::RemoveChannel(FMOD::Channel* _pTargetChannel)
{
	list<FMOD::Channel*>::iterator iter = m_listChannel.begin();
	for (; iter != m_listChannel.end(); ++iter)
	{
		if (*iter == _pTargetChannel)
		{
			m_listChannel.erase(iter);
			return;
		}
	}
}

void CSound::Load(const wstring& _strFilePath)
{
	string path(_strFilePath.begin(), _strFilePath.end());
	
	wstring strFullPath = CPathMgr::GetResPath();
	string fullPath(strFullPath.begin(), strFullPath.end());

	fullPath += path;

	if (FMOD_OK != g_pFMOD->createSound(fullPath.c_str(), FMOD_DEFAULT, nullptr, &m_pSound))
	{
		assert(nullptr);
	}
}
//
///////////////////////////////
//
//FMOD_SYSTEM* SJ_pSystem;
//FMOD_SOUND* SJ_pSound[SD_END];
//FMOD_CHANNEL* SJ_pChannel[SD_END];
//
//FMOD_SYSTEM* effg_pSystem;  //
//FMOD_SOUND* effg_pSound[EFFSD_END];
//FMOD_CHANNEL* effg_pChannel[EFFSD_END];
//int pause;
//void soundSetup()
//{
//	char str[128];
//
//	//사운드 시스템 생성
//	FMOD_System_Create(&SJ_pSystem);
//
//	//채널수, 모드, 0
//	FMOD_System_Init(SJ_pSystem, 1, FMOD_INIT_NORMAL, NULL);
//
//	//사운드 경로
//	for (int i = 0; i < SD_END; i++) {
//		wsprintfA(str, "sound\\STAGE%d.mp3", i);
//		FMOD_System_CreateStream(SJ_pSystem, str, FMOD_LOOP_NORMAL, 0, &SJ_pSound[i]);
//	}
//
//}
//void playSound(ESOUNDKIND eSound)
//{
//	FMOD_System_PlaySound(SJ_pSystem, FMOD_CHANNEL_FREE, SJ_pSound[eSound], 0, &SJ_pChannel[eSound]);
//
//}
//
//void effSoundSetup()
//{
//	char str[128];
//
//	//사운드 시스템 생성
//	FMOD_System_Create(&effg_pSystem);
//
//	//채널수, 모드, 0
//	FMOD_System_Init(effg_pSystem, 1, FMOD_INIT_NORMAL, NULL);
//
//	//사운드 경로
//
//	for (int i = 0; i < 4; i++) {
//		wsprintfA(str, "effect\\effect%d.mp3", i);
//		FMOD_System_CreateStream(effg_pSystem, str, FMOD_LOOP_OFF, 0, &effg_pSound[i]);
//	}
//}
//void effSoundoff()
//{
//	for (int i = 0; i < EFFSD_END; i++)
//		FMOD_Channel_Stop(effg_pChannel[i]);
//}
//void effPlaySound(EFFOUNDKIND eSound)
//{
//	FMOD_System_PlaySound(effg_pSystem, FMOD_CHANNEL_FREE, effg_pSound[eSound], 0, &effg_pChannel[eSound]);
//}
//
//void SoundPause()
//{
//	for (int i = 0; i < SD_END; i++)
//		FMOD_Channel_GetPaused(SJ_pChannel[i], &pause);
//	for (int i = 0; i < SD_END; i++)
//		FMOD_Channel_SetPaused(SJ_pChannel[i], !pause);
//}

//FMOD_SYSTEM* CBGM::g_sound_system;
//
//CBGM::CBGM(const char* path, bool loop) {
//    	
//    wstring strFullPath = CPathMgr::GetResPath();
//    string fullPath(strFullPath.begin(), strFullPath.end());
//    fullPath += string(path);
//
//
//	std::vector<char> v(fullPath.begin(), fullPath.end());
//	v.push_back('\0');
//	char* finalPath = &v[0];
//
//	int a = 3;
//
//    if (loop) {
//        FMOD_System_CreateSound(g_sound_system, path, FMOD_LOOP_NORMAL, 0, &m_sound);
//    }
//    else {
//        FMOD_System_CreateSound(g_sound_system, path, FMOD_DEFAULT, 0, &m_sound);
//		char* c = finalPath;
//    }
//
//    m_channel = nullptr;
//    m_volume = SOUND_DEFAULT;
//}
//
//CBGM::~CBGM() {
//    FMOD_Sound_Release(m_sound);
//}
//
//
//int CBGM::Init() {
//    FMOD_System_Create(&g_sound_system);
//    FMOD_System_Init(g_sound_system, 32, FMOD_INIT_NORMAL, NULL);
//
//    return 0;
//}
//
//int CBGM::Release() {
//    FMOD_System_Close(g_sound_system);
//    FMOD_System_Release(g_sound_system);
//
//    return 0;
//}
//
//
//int CBGM::play() {
//    FMOD_System_PlaySound(g_sound_system, m_sound, NULL, false, &m_channel);
//
//    return 0;
//}
//
//int CBGM::pause() {
//    FMOD_Channel_SetPaused(m_channel, true);
//
//    return 0;
//}
//
//int CBGM::resume() {
//    FMOD_Channel_SetPaused(m_channel, false);
//
//    return 0;
//}
//
//int CBGM::stop() {
//    FMOD_Channel_Stop(m_channel);
//
//    return 0;
//}
//
//int CBGM::volumeUp() {
//    if (m_volume < SOUND_MAX) {
//        m_volume += SOUND_WEIGHT;
//    }
//
//    FMOD_Channel_SetVolume(m_channel, m_volume);
//
//    return 0;
//}
//
//int CBGM::volumeDown() {
//    if (m_volume > SOUND_MIN) {
//        m_volume -= SOUND_WEIGHT;
//    }
//
//    FMOD_Channel_SetVolume(m_channel, m_volume);
//
//    return 0;
//}
//
//
//int CBGM::Update() {
//    FMOD_Channel_IsPlaying(m_channel, &m_bool);
//
//    if (m_bool) {
//        FMOD_System_Update(g_sound_system);
//    }
//
//    return 0;
//}