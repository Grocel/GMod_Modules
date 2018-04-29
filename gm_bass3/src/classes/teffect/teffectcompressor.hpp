#include "../teffect.hpp"

#ifndef T_EFFECT_COMPRESSOR_H
#define T_EFFECT_COMPRESSOR_H

class TEffectCompressor : public TEffect
{
	GETSETLIMIT(float, fGain, Gain, 0, -60, 60);
	GETSETLIMIT(float, fAttack, Attack, 10, 0.01, 500);
	GETSETLIMIT(float, fRelease, Release, 200, 50, 3000);
	GETSETLIMIT(float, fThreshold, Threshold, -20, -60, 0);
	GETSETLIMIT(float, fRatio, Ratio, 3, 1, 100);
	GETSETLIMIT(float, fPredelay, Predelay, 4, 0, 4);

protected:
	BASS_DX8_COMPRESSOR* pData = NULL;
	void* GetData();

public:
	static string LUAMETANAME;
	static int LUAMETAID;

	TEffectCompressor();
	~TEffectCompressor();

	string ToString();
	operator string();

	friend ostream& operator<<(ostream& os, TEffectCompressor& FX);
};
#endif