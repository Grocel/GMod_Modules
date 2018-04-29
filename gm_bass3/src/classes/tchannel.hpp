#include <thread>
#include <mutex>
#include <list>

#include "bass/bass.h"
#include "tluarefable.hpp"
#include "teffect.hpp"

#ifndef T_CHANNEL_H
#define T_CHANNEL_H



class TChannel : public TLuaRefAble
{

private:
// +--------------------------------------------------+
// |                Private Variables                 |
// +--------------------------------------------------+
	bass_p pHandle;

	bool bIsOnline;
	bool bCanSeek;
	bool bSeaking;

	bass_time iSeekingTo;

	thread* pthSeeker;
	string sFilename;
	mutex MutexLoadingLock;

	list<TEffect*> lsFX;

	float fVolumeBoost;
	float fVolume;

// +--------------------------------------------------+
// |                 Private Methods                  |
// +--------------------------------------------------+
	void Init();
	bool IsValidInternal();
	bool Is3DInternal();
	void RemoveInternal();

	void ClearFxInternal();
	void ApplyFxInternal();
	void UpdateFxInternal();

	void Reset3D();

	bass_time TimeToByte(double fTime);
	double ByteToTime(bass_time iBytes);
	bass_time GetRawTime();
	void SetRawTime(bass_time iTime);

	bool HasFlag(bass_flag eFlag);
	void SetFlag(bass_flag eF, bool bVar);

	int LoadURL(string sURL, bass_flag eFlags);
	int LoadFile(string sURL, bass_flag eFlags);


public:
	static string LUAMETANAME;
	static int LUAMETAID;
	static map<unsigned long long, TChannel*> g_mapObjectInstances;

// +--------------------------------------------------+
// |              Constructor/Destructor              |
// +--------------------------------------------------+
	TChannel();
	TChannel(string sURL, bool bIsOnline, bass_flag eFlags);
	TChannel(string sURL, bool bIsOnline);
	~TChannel();
	
// +--------------------------------------------------+
// |                  Public Methods                  |
// +--------------------------------------------------+
	bass_p GetBassHandle();

	bool IsValid();
	void Remove();
	bool Update(DWORD ilength);
	bool Update();
	void Think();

	int Load(string sURL, bool bIsOnline, bass_flag eFlags);
	int Load(string sURL, bool bIsOnline);

	void Play(bool bRestart);
	void Play();
	void Stop();
	void Pause();

	bass_flag GetState();

	void SetVolume(float fVolume);
	float GetVolume();
	void SetVolumeBoost(float fVolumeBoost);
	float GetVolumeBoost();
	void VolumeFadeTo(float fVolume, DWORD iTime);
	bool VolumeIsFading();

	void SetBalance(float fBalance);
	float GetBalance();
	void BalanceFadeTo(float fBalance, DWORD iTime);
	bool BalanceIsFading();

	bool Is3D();
	bool IsLooping();
	bool IsOnline();
	bool IsBlockStreamed();

	void EnableLooping(bool bLoop);
	bool FFT(bass_flag eMode, float *pfSpectrum);
	bool GetLevel(WORD* piLevelLeft, WORD* piLevelRight);

	bool GetLevelEx(float* pfLevels);
	bool GetLevelEx(float* pfLevels, float fTimeFrame);
	bool GetLevelEx(float* pfLevels, float fTimeFrame, bool bRMS);

	double GetTime();

	void SetTime(double fTime);

	double GetLength();
	bool IsEndless();

	bool Seek();
	bool IsSeeking();
	bool CanSeek();

	bass_time GetSeekingTo();

	const char* GetTag(bass_flag eMode);
	string GetFileName();
	string GetFileFormat();


	DWORD GetSamplingRate();
	BYTE GetBitsPerSample();
	float GetAverageBitRate();

	bool GetPos(BASS_3DVECTOR* pvPos, BASS_3DVECTOR* pvDir, BASS_3DVECTOR* pvVel);
	bool SetPos(BASS_3DVECTOR* pvPos, BASS_3DVECTOR* pvDir, BASS_3DVECTOR* pvVel);

	bool GetPos(BASS_3DVECTOR* pvPos, BASS_3DVECTOR* pvDir);
	bool SetPos(BASS_3DVECTOR* pvPos, BASS_3DVECTOR* pvDir);

	bool GetPos(BASS_3DVECTOR* pvPos);
	bool SetPos(BASS_3DVECTOR* pvPos);

	bool Get3DFadeDistance(float* pfMin, float* pfMax);
	void Set3DFadeDistance(float fMin, float fMax);

	bool Get3DFadeDistance(float* pfMin);
	void Set3DFadeDistance(float fMin);

	bool Get3DCone(DWORD* piInnerAngle, DWORD* piOuterAngle, float* pfOuterVolume);
	void Set3DCone(DWORD iInnerAngle, DWORD iOuterAngle, float fOuterVolume);

	bool Get3DCone(DWORD* piInnerAngle, DWORD* piOuterAngle);
	void Set3DCone(DWORD iInnerAngle, DWORD iOuterAngle);

	bool Get3DEnabled();
	void Set3DEnabled(bool bEnabled);

	float GetEAXMix();
	void SetEAXMix(float fMix);
	
	bool AddFx(TEffect* pFX);
	void ClearFx();

	string ToString();
	operator string();

	operator bass_p();
	bool operator ==(TChannel& other);

	// +--------------------------------------------------+
	// |                    Friends                       |
	// +--------------------------------------------------+
	friend void thfnSeekTo(TChannel* pChannel);

	friend ostream& operator<<(ostream& os, TChannel& Channel);
};
#endif