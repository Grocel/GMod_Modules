#include "../teffect.hpp"

#ifndef T_EFFECT_ECHO_H
#define T_EFFECT_ECHO_H

class TEffectEcho : public TEffect
{
	GETSETLIMIT(float, fWetDryMix, WetDryMix, 50, 0, 100);
	GETSETLIMIT(float, fFeedback, Feedback, 50, 0, 100);
	GETSETLIMIT(float, fLeftDelay, LeftDelay, 500, 1, 2000);
	GETSETLIMIT(float, fRightDelay, RightDelay, 500, 1, 2000);
	GETSETBOOL(lPanDelay, PanDelay, FALSE);

protected:
	BASS_DX8_ECHO* pData = NULL;
	void* GetData();

public:
	static string LUAMETANAME;
	static int LUAMETAID;

	TEffectEcho();
	~TEffectEcho();

	string ToString();
	operator string();

	friend ostream& operator<<(ostream& os, TEffectEcho& FX);
};
#endif