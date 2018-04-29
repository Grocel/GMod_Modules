#include "teffecti3dl2reverb.hpp"

string TEffectI3DL2Reverb::LUAMETANAME = "IBASS3FxI3DL2Reverb";
int TEffectI3DL2Reverb::LUAMETAID = 0;

void* TEffectI3DL2Reverb::GetData()
{
	if (ISNULLPTR(pData))
	{
		pData = new BASS_DX8_I3DL2REVERB();
	}

	if (ISNULLPTR(pData))
	{
		return NULL;
	}

	pData->lRoom = this->GetRoom();
	pData->lRoomHF = this->GetRoomHF();
	pData->flRoomRolloffFactor = this->GetRoomRolloffFactor();
	pData->flDecayTime = this->GetDecayTime();

	pData->flDecayHFRatio = this->GetDecayHFRatio();
	pData->lReflections = this->GetReflections();
	pData->flReflectionsDelay = this->GetReflectionsDelay();
	pData->lReverb = this->GetReverb();

	pData->flReverbDelay = this->GetReverbDelay();
	pData->flDiffusion = this->GetDiffusion();
	pData->flDensity = this->GetDensity();
	pData->flHFReference = this->GetHFReference();

	return pData;
}

TEffectI3DL2Reverb::TEffectI3DL2Reverb()
{
	pData = NULL;
	eType = BASS_FX_DX8_I3DL2REVERB;
}

TEffectI3DL2Reverb::~TEffectI3DL2Reverb()
{
	if (ISNULLPTR(pData)) return;
	delete pData;
}

string TEffectI3DL2Reverb::ToString()
{
	return string(*this);
}

TEffectI3DL2Reverb::operator string()
{
	stringstream out;

	out << (*this);

	return string(out.str());
}

ostream& operator<<(ostream& os, TEffectI3DL2Reverb& FX)
{
	if (!FX.IsValid())
	{
		os << "[NULL " << TEffectI3DL2Reverb::LUAMETANAME << "]";
		return os;
	}

	os << TEffectI3DL2Reverb::LUAMETANAME << ": " << &FX;
	return os;
}
