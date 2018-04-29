#include "../teffect.hpp"

#ifndef T_EFFECT_I3DL2REVERB_H
#define T_EFFECT_I3DL2REVERB_H

class TEffectI3DL2Reverb : public TEffect
{
	GETSETLIMIT(int, lRoom, Room, -1000, -10000, 0);
	GETSETLIMIT(int, lRoomHF, RoomHF, -100, -10000, 0);
	GETSETLIMIT(float, flRoomRolloffFactor, RoomRolloffFactor, 0, 0, 10);
	GETSETLIMIT(float, flDecayTime, DecayTime, 1.49, 0.1, 20);

	GETSETLIMIT(float, flDecayHFRatio, DecayHFRatio, 0.83, 0.1, 2);
	GETSETLIMIT(int, lReflections, Reflections, -2602, -10000, 1000);
	GETSETLIMIT(float, flReflectionsDelay, ReflectionsDelay, 0.007, 0, 0.3);
	GETSETLIMIT(int, lReverb, Reverb, 200, -10000, 2000);

	GETSETLIMIT(float, flReverbDelay, ReverbDelay, 0.011, 0, 0.1);
	GETSETLIMIT(float, flDiffusion, Diffusion, 100, 0, 100);
	GETSETLIMIT(float, flDensity, Density, 100, 0, 100);
	GETSETLIMIT(float, flHFReference, HFReference, 5000, 20, 20000);

protected:
	BASS_DX8_I3DL2REVERB* pData = NULL;
	void* GetData();

public:
	static string LUAMETANAME;
	static int LUAMETAID;

	TEffectI3DL2Reverb();
	~TEffectI3DL2Reverb();

	string ToString();
	operator string();

	friend ostream& operator<<(ostream& os, TEffectI3DL2Reverb& FX);
};
#endif