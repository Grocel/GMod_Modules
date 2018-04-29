#include "../teffect.hpp"

#ifndef T_EFFECT_VOLUME_H
#define T_EFFECT_VOLUME_H

class TEffectVolume : public TEffect, public TEffectTrait_Curve
{
	GETSETLIMIT(float, fTarget, Target, 1, 0, 1000);
	GETSETLIMIT_WITH_NOCHANGE(float, fCurrent, Current, 1, 0, 1000);
	GETSETLIMIT(float, fTime, Time, 0, 0, 3600);

protected:
	BASS_FX_VOLUME_PARAM* pData = NULL;
	void* GetData();

public:
	static string LUAMETANAME;
	static int LUAMETAID;

	TEffectVolume();
	~TEffectVolume();

	string ToString();
	operator string();

	friend ostream& operator<<(ostream& os, TEffectVolume& FX);
};
#endif