#include "teffectflanger.hpp"

string TEffectFlanger::LUAMETANAME = "IBASS3FxFlanger";
int TEffectFlanger::LUAMETAID = 0;

void* TEffectFlanger::GetData()
{
	if (ISNULLPTR(pData))
	{
		pData = new BASS_DX8_FLANGER();
	}

	if (ISNULLPTR(pData))
	{
		return NULL;
	}

	pData->fWetDryMix = this->GetWetDryMix();
	pData->fDepth = this->GetDepth();
	pData->fFeedback = this->GetFeedback();
	pData->fFrequency = this->GetFrequency();
	pData->lWaveform = this->GetWaveform();
	pData->fDelay = this->GetDelay();
	pData->lPhase = this->GetPhase();

	return pData;
}

TEffectFlanger::TEffectFlanger()
{
	pData = NULL;
	eType = BASS_FX_DX8_FLANGER;

	eWaveformDefault = BASS_WAVEFORM_SINE;
	eWaveform.store(eWaveformDefault);

	ePhaseDefault = BASS_DX8_PHASE_ZERO;
	ePhase.store(ePhaseDefault);
}

TEffectFlanger::~TEffectFlanger()
{
	if (ISNULLPTR(pData)) return;
	delete pData;
}

string TEffectFlanger::ToString()
{
	return string(*this);
}

TEffectFlanger::operator string()
{
	stringstream out;

	out << (*this);

	return string(out.str());
}

ostream& operator<<(ostream& os, TEffectFlanger& FX)
{
	if (!FX.IsValid())
	{
		os << "[NULL " << TEffectFlanger::LUAMETANAME << "]";
		return os;
	}

	os << TEffectFlanger::LUAMETANAME << ": " << &FX;
	return os;
}
