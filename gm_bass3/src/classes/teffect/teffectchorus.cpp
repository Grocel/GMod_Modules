#include "teffectchorus.hpp"

string TEffectChorus::LUAMETANAME = "IBASS3FxChorus";
int TEffectChorus::LUAMETAID = 0;

void* TEffectChorus::GetData()
{
	if (ISNULLPTR(pData))
	{
		pData = new BASS_DX8_CHORUS();
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

TEffectChorus::TEffectChorus()
{
	pData = NULL;
	eType = BASS_FX_DX8_CHORUS;

	eWaveformDefault = BASS_WAVEFORM_SINE;
	eWaveform.store(eWaveformDefault);

	ePhaseDefault = BASS_DX8_PHASE_90;
	ePhase.store(ePhaseDefault);
}

TEffectChorus::~TEffectChorus()
{
	if (ISNULLPTR(pData)) return;
	delete pData;
}

string TEffectChorus::ToString()
{
	return string(*this);
}

TEffectChorus::operator string()
{
	stringstream out;

	out << (*this);

	return string(out.str());
}

ostream& operator<<(ostream& os, TEffectChorus& FX)
{
	if (!FX.IsValid())
	{
		os << "[NULL " << TEffectChorus::LUAMETANAME << "]";
		return os;
	}

	os << TEffectChorus::LUAMETANAME << ": " << &FX;
	return os;
}
