#include "teffectgargle.hpp"

string TEffectGargle::LUAMETANAME = "IBASS3FxGargle";
int TEffectGargle::LUAMETAID = 0;

void* TEffectGargle::GetData()
{
	if (ISNULLPTR(pData))
	{
		pData = new BASS_DX8_GARGLE();
	}

	if (ISNULLPTR(pData))
	{
		return NULL;
	}

	pData->dwRateHz = this->GetRate();
	pData->dwWaveShape = this->GetWaveform();

	return pData;
}

TEffectGargle::TEffectGargle()
{
	pData = NULL;
	eType = BASS_FX_DX8_GARGLE;

	eWaveformDefault = BASS_WAVEFORM_TRIANGLE;
	eWaveform.store(eWaveformDefault);
}

TEffectGargle::~TEffectGargle()
{
	if (ISNULLPTR(pData)) return;
	delete pData;
}

string TEffectGargle::ToString()
{
	return string(*this);
}

TEffectGargle::operator string()
{
	stringstream out;

	out << (*this);

	return string(out.str());
}

ostream& operator<<(ostream& os, TEffectGargle& FX)
{
	if (!FX.IsValid())
	{
		os << "[NULL " << TEffectGargle::LUAMETANAME << "]";
		return os;
	}

	os << TEffectGargle::LUAMETANAME << ": " << &FX;
	return os;
}
