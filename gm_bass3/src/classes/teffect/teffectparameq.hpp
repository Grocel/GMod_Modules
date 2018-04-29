#include "../teffect.hpp"

#ifndef T_EFFECT_PARAMEQ_H
#define T_EFFECT_PARAMEQ_H

class TEffectParamEQ : public TEffect
{
	GETSETLIMIT(float, fBandwidth, Bandwidth, 12, 1, 36);
	GETSETLIMIT(float, fGain, Gain, 0, -15, 15);

protected:
	atomic<float> fCenter = { 80 };

	BASS_DX8_PARAMEQ* pData = NULL;
	void* GetData();

public:
	static string LUAMETANAME;
	static int LUAMETAID;

	TEffectParamEQ();
	~TEffectParamEQ();

	float GetCenter();
	void SetCenter(float fCenter);

	string ToString();
	operator string();

	friend ostream& operator<<(ostream& os, TEffectParamEQ& FX);
};
#endif