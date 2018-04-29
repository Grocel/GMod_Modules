#include "teffectparameq.hpp"

string TEffectParamEQ::LUAMETANAME = "IBASS3FxParamEQ";
int TEffectParamEQ::LUAMETAID = 0;

void* TEffectParamEQ::GetData()
{
	if (ISNULLPTR(pData))
	{
		pData = new BASS_DX8_PARAMEQ();
	}

	if (ISNULLPTR(pData))
	{
		return NULL;
	}

	pData->fCenter = this->GetCenter();
	pData->fBandwidth = this->GetBandwidth();
	pData->fGain = this->GetGain();

	return pData;
}

TEffectParamEQ::TEffectParamEQ()
{
	pData = NULL;
	eType = BASS_FX_DX8_PARAMEQ;
}

TEffectParamEQ::~TEffectParamEQ()
{
	if (ISNULLPTR(pData)) return;
	delete pData;
}

float TEffectParamEQ::GetCenter()
{
	float fCenter = this->fCenter.load();

	if (pChannel == BASS_NULL)
	{
		return fCenter;
	}

	float fSampleRate = 0;

	if (BASS_ChannelGetAttribute(pChannel, BASS_ATTRIB_FREQ, &fSampleRate) == FALSE)
	{
		return fCenter;
	}

	if (fSampleRate <= 0)
	{
		fSampleRate = 44100;
	}
	
	// Plattform specific limits, see: http://www.un4seen.com/doc/#bass/BASS_DX8_PARAMEQ.html
#ifdef _WIN32
	float fMinFrq = 80;
	float fMaxFrq = fSampleRate / 3;

	if (fMaxFrq > 16000) fMaxFrq = 16000;
#else
	float fMinFrq = 0;
	float fMaxFrq = fSampleRate / 2;
#endif

	if (fCenter < fMinFrq) fCenter = fMinFrq;
	if (fCenter > fMaxFrq) fCenter = fMaxFrq;

	return fCenter;
}

void TEffectParamEQ::SetCenter(float fCenter)
{
	this->fCenter.store(fCenter);
}

string TEffectParamEQ::ToString()
{
	return string(*this);
}

TEffectParamEQ::operator string()
{
	stringstream out;

	out << (*this);

	return string(out.str());
}

ostream& operator<<(ostream& os, TEffectParamEQ& FX)
{
	if (!FX.IsValid())
	{
		os << "[NULL " << TEffectParamEQ::LUAMETANAME << "]";
		return os;
	}

	os << TEffectParamEQ::LUAMETANAME << ": " << &FX;
	return os;
}
