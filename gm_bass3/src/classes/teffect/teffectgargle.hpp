#include "../teffect.hpp"

#ifndef T_EFFECT_GARGLE_H
#define T_EFFECT_GARGLE_H

class TEffectGargle : public TEffect, public TEffectTrait_Waveform_TQ
{
	GETSETLIMIT(DWORD, dwRateHz, Rate, 20, 1, 1000);

protected:
	BASS_DX8_GARGLE* pData = NULL;
	void* GetData();

public:
	static string LUAMETANAME;
	static int LUAMETAID;

	TEffectGargle();
	~TEffectGargle();

	string ToString();
	operator string();

	friend ostream& operator<<(ostream& os, TEffectGargle& FX);
};
#endif