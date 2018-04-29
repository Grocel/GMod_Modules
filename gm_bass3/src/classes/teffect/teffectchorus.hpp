#include "../teffect.hpp"

#ifndef T_EFFECT_CHORUS_H
#define T_EFFECT_CHORUS_H

class TEffectChorus : public TEffect, public TEffectTrait_Waveform_TS, public TEffectTrait_Phase
{
	GETSETLIMIT(float, fWetDryMix, WetDryMix, 50, 0, 100);
	GETSETLIMIT(float, fDepth, Depth, 10, 0, 100);
	GETSETLIMIT(float, fFeedback, Feedback, 25, -99, 99);
	GETSETLIMIT(float, fFrequency, Frequency, 1.1, 0, 10);
	GETSETLIMIT(float, fDelay, Delay, 16, 0, 20);

protected:
	BASS_DX8_CHORUS* pData = NULL;
	void* GetData();

public:
	static string LUAMETANAME;
	static int LUAMETAID;

	TEffectChorus();
	~TEffectChorus();

	string ToString();
	operator string();

	friend ostream& operator<<(ostream& os, TEffectChorus& FX);
};
#endif