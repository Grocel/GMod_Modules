#include "teffectreverb.hpp"

string TEffectReverb::LUAMETANAME = "IBASS3FxReverb";
int TEffectReverb::LUAMETAID = 0;

void* TEffectReverb::GetData()
{
	if (ISNULLPTR(pData))
	{
		pData = new BASS_DX8_REVERB();
	}

	if (ISNULLPTR(pData))
	{
		return NULL;
	}

	pData->fInGain = this->GetInGain();
	pData->fReverbMix = this->GetReverbMix();
	pData->fReverbTime = this->GetReverbTime();
	pData->fHighFreqRTRatio = this->GetHighFreqRTRatio();

	return pData;
}

TEffectReverb::TEffectReverb()
{
	pData = NULL;
	eType = BASS_FX_DX8_REVERB;
}

TEffectReverb::~TEffectReverb()
{
	if (ISNULLPTR(pData)) return;
	delete pData;
}

string TEffectReverb::ToString()
{
	return string(*this);
}

TEffectReverb::operator string()
{
	stringstream out;

	out << (*this);

	return string(out.str());
}

ostream& operator<<(ostream& os, TEffectReverb& FX)
{
	if (!FX.IsValid())
	{
		os << "[NULL " << TEffectReverb::LUAMETANAME << "]";
		return os;
	}

	os << TEffectReverb::LUAMETANAME << ": " << &FX;
	return os;
}
