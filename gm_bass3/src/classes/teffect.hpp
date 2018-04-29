#include "bass/bass.h"
#include "tluarefable.hpp"

#ifndef T_EFFECT_H
#define T_EFFECT_H



#define GETSET(TYPE, VALUENAME, NAME, DEFAULT) \
	private: \
		atomic<TYPE> VALUENAME = { ( TYPE )( DEFAULT ) }; \
	public: \
		TYPE Get##NAME() { \
			return this->VALUENAME.load(); \
		} \
		void Set##NAME( TYPE in ) { \
			if (bIsRemoved) return; \
			this->VALUENAME.store( in ); \
			this->bUpdated = true; \
		}

#define GETSETLIMIT(TYPE, VALUENAME, NAME, DEFAULT, MINV, MAXV) \
	private: \
		atomic<TYPE> VALUENAME = { ( TYPE )( DEFAULT ) }; \
	public: \
		TYPE Get##NAME() { \
			return this->VALUENAME.load(); \
		} \
		void Set##NAME( TYPE in ) { \
			if (bIsRemoved) return; \
			if ( in < MINV ) in = MINV; \
			if ( in > MAXV ) in = MAXV; \
			this->VALUENAME.store( in ); \
			this->bUpdated = true; \
		}

#define GETSETLIMIT_WITH_NOCHANGE(TYPE, VALUENAME, NAME, DEFAULT, MINV, MAXV) \
	private: \
		atomic<TYPE> VALUENAME = { ( TYPE )( DEFAULT ) }; \
	public: \
		TYPE Get##NAME() { \
			return this->VALUENAME.load(); \
		} \
		void Set##NAME( TYPE in ) { \
			if (bIsRemoved) return; \
			if ( in <= BASS_NO_CHANGE ) { \
				in = BASS_NO_CHANGE; \
			} else { \
				if (in < MINV) in = MINV; \
			} \
			if ( in > MAXV ) in = MAXV; \
			this->VALUENAME.store( in ); \
			this->bUpdated = true; \
		}


#define GETSETBOOL(VALUENAME, NAME, DEFAULT) \
	private: \
		atomic<BOOL> VALUENAME = { ( BOOL )( DEFAULT ) }; \
	public: \
		bool Get##NAME() { \
			return ( BOOL )( this->VALUENAME.load() ) == ( BOOL )( TRUE ); \
		} \
		void Set##NAME( bool in ) { \
			if (bIsRemoved) return; \
			this->VALUENAME.store( in ? TRUE : FALSE ); \
			this->bUpdated = true; \
		} \
		void Set##NAME( BOOL in ) { \
			if (bIsRemoved) return; \
			this->VALUENAME.store( ( ( BOOL )( in ) == ( BOOL )( TRUE ) ) ? TRUE : FALSE ); \
			this->bUpdated = true; \
		}

class TEffect : public TLuaRefAble
{

protected:
	bass_p pChannel;
	bass_fx pFX;
	bass_flag eType;

	int iErrorCode;
	bool bEnabled;
	bool bUpdated;

	virtual void* GetData();
	void ResetInternal();
	void UpdateInternal();
	bool ApplyInternal(bass_p pChannel);

	void RemoveFX();

public:
	static string LUAMETANAME;
	static int LUAMETAID;

	TEffect();
	~TEffect();

	virtual bool IsValid();
	void Reset();

	bool Apply();
	bool Apply(bass_p pChannel);

	void Update();
	void Remove();

	const int GetError() const;

	void SetEnabled(bool bEnabled);
	const bool GetEnabled() const;

	virtual string ToString();
	virtual operator string();

	friend ostream& operator<<(ostream& os, TEffect& FX);
};

class TEffectTrait_Waveform_TS
{

protected:
	atomic<bass_flag> eWaveform = { BASS_WAVEFORM_TRIANGLE };
	bass_flag eWaveformDefault = BASS_WAVEFORM_TRIANGLE;

public:
	virtual bass_flag GetWaveform();
	virtual void SetWaveform(bass_flag eWaveform);

};

class TEffectTrait_Waveform_TQ : public TEffectTrait_Waveform_TS
{
public:
	void SetWaveform(bass_flag eWaveform);
};


class TEffectTrait_Phase
{

protected:
	atomic<bass_flag> ePhase = { BASS_DX8_PHASE_ZERO };
	bass_flag ePhaseDefault = BASS_DX8_PHASE_ZERO;

public:
	bass_flag GetPhase();
	void SetPhase(bass_flag ePhase);

};

class TEffectTrait_Curve
{

protected:
	atomic<bass_flag> eCurve = { BASS_CURVE_LINEAR };
	bass_flag eCurveDefault = BASS_CURVE_LINEAR;

public:
	bass_flag GetCurve();
	void SetCurve(bass_flag eCurve);

};
#endif