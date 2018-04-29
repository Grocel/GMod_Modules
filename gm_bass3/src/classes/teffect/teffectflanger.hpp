#include "../teffect.hpp"

#ifndef T_EFFECT_FLANGER_H
#define T_EFFECT_FLANGER_H

class TEffectFlanger : public TEffect, public TEffectTrait_Waveform_TS, public TEffectTrait_Phase
{
	GETSETLIMIT(float, fWetDryMix, WetDryMix, 50, 0, 100);
	GETSETLIMIT(float, fDepth, Depth, 100, 0, 100);
	GETSETLIMIT(float, fFeedback, Feedback, -50, -99, 99);
	GETSETLIMIT(float, fFrequency, Frequency, 0.25, 0, 10);
	GETSETLIMIT(float, fDelay, Delay, 2, 0, 4);

protected:
	BASS_DX8_FLANGER* pData = NULL;
	void* GetData();

public:
	static string LUAMETANAME;
	static int LUAMETAID;

	TEffectFlanger();
	~TEffectFlanger();

	string ToString();
	operator string();

	friend ostream& operator<<(ostream& os, TEffectFlanger& FX);
};
#endif