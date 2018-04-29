#include "teffectdistortion.hpp"

string TEffectDistortion::LUAMETANAME = "IBASS3FxDistortion";
int TEffectDistortion::LUAMETAID = 0;

void* TEffectDistortion::GetData()
{
	if (ISNULLPTR(pData))
	{
		pData = new BASS_DX8_DISTORTION();
	}

	if (ISNULLPTR(pData))
	{
		return NULL;
	}

	pData->fGain = this->GetGain();
	pData->fEdge = this->GetEdge();
	pData->fPostEQCenterFrequency = this->GetPostEQCenterFrequency();
	pData->fPostEQBandwidth = this->GetPostEQBandwidth();
	pData->fPreLowpassCutoff = this->GetPreLowpassCutoff();

	return pData;
}

TEffectDistortion::TEffectDistortion()
{
	pData = NULL;
	eType = BASS_FX_DX8_DISTORTION;
}

TEffectDistortion::~TEffectDistortion()
{
	if (ISNULLPTR(pData)) return;
	delete pData;
}

string TEffectDistortion::ToString()
{
	return string(*this);
}

TEffectDistortion::operator string()
{
	stringstream out;

	out << (*this);

	return string(out.str());
}

ostream& operator<<(ostream& os, TEffectDistortion& FX)
{
	if (!FX.IsValid())
	{
		os << "[NULL " << TEffectDistortion::LUAMETANAME << "]";
		return os;
	}

	os << TEffectDistortion::LUAMETANAME << ": " << &FX;
	return os;
}
