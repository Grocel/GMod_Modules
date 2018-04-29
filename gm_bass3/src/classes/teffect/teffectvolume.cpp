#include "teffectvolume.hpp"

string TEffectVolume::LUAMETANAME = "IBASS3FxVolume";
int TEffectVolume::LUAMETAID = 0;

void* TEffectVolume::GetData()
{
	if (ISNULLPTR(pData))
	{
		pData = new BASS_FX_VOLUME_PARAM();
	}

	if (ISNULLPTR(pData))
	{
		return NULL;
	}

	pData->fTarget = this->GetTarget();
	pData->fCurrent = this->GetCurrent();
	pData->fTime = this->GetTime();
	pData->lCurve = this->GetCurve();

	return pData;
}

TEffectVolume::TEffectVolume()
{
	pData = NULL;
	eType = BASS_FX_VOLUME;

	eCurveDefault = BASS_CURVE_LINEAR;
	eCurve.store(eCurveDefault);
}

TEffectVolume::~TEffectVolume()
{
	if (ISNULLPTR(pData)) return;
	delete pData;
}

string TEffectVolume::ToString()
{
	return string(*this);
}

TEffectVolume::operator string()
{
	stringstream out;

	out << (*this);

	return string(out.str());
}

ostream& operator<<(ostream& os, TEffectVolume& FX)
{
	if (!FX.IsValid())
	{
		os << "[NULL " << TEffectVolume::LUAMETANAME << "]";
		return os;
	}

	os << TEffectVolume::LUAMETANAME << ": " << &FX;
	return os;
}
