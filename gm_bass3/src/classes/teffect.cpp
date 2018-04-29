#include "teffect.hpp"

string TEffect::LUAMETANAME = "IBASS3Fx";
int TEffect::LUAMETAID = 0;

void* TEffect::GetData()
{
	return NULL;
}

void TEffect::ResetInternal()
{
	bUpdated = true;

	if (pChannel != BASS_NULL)
	{
		if (BASS_FXReset(pChannel) == TRUE) return;
	}

	if (pFX != BASS_NULL)
	{
		BASS_FXReset(pFX);
	}
}

void TEffect::UpdateInternal()
{
	if (bIsRemoved) return;
	if (pFX == BASS_NULL) return;

	if (!bEnabled)
	{
		RemoveFX();
		return;
	}

	if (!bUpdated) return;

	void* pParamData = this->GetData();
	if (ISNULLPTR(pParamData)) return;

	BASS_FXSetParameters(pFX, pParamData);
	iErrorCode = BASS_ErrorGetCode();

	bUpdated = (iErrorCode == BASS_OK);
}

bool TEffect::ApplyInternal(bass_p pChannel)
{
	if (IsRemoved()) return false;
	bUpdated = true;

	iErrorCode = BASS_OK;

	if (eType == BASS_FX_UNKNOWN)
	{
		iErrorCode = BASS_ERROR_ILLPARAM;
		return false;
	}

	if (pChannel == BASS_NULL)
	{
		iErrorCode = BASS_ERROR_HANDLE;
		return false;
	}

	bass_p pOldChannel = this->pChannel;

	if (pOldChannel != pChannel)
	{
		this->pChannel = pChannel;

		if (pOldChannel != BASS_NULL && pFX != BASS_NULL)
		{
			BASS_ChannelRemoveFX(pOldChannel, pFX);
			pFX = BASS_NULL;
		}
	}
	else
	{
		if (pFX != BASS_NULL && bEnabled)
		{
			return true;
		}
	}

	if (!bEnabled)
	{
		RemoveFX();
		return true;
	}

	pFX = BASS_ChannelSetFX(pChannel, eType, 0);
	iErrorCode = BASS_ErrorGetCode();

	if (iErrorCode != BASS_OK)
	{
		RemoveFX();
		return false;
	}

	UpdateInternal();
	ResetInternal();
	return true;
}

void TEffect::RemoveFX()
{
	if (pChannel != BASS_NULL && pFX != BASS_NULL)
	{
		BASS_ChannelRemoveFX(pChannel, pFX);
	}

	ResetInternal();

	pFX = BASS_NULL;
	bUpdated = true;
}

TEffect::TEffect()
{
	eType = BASS_FX_UNKNOWN;
	pChannel = BASS_NULL;
	pFX = BASS_NULL;
	iErrorCode = BASS_OK;
	bEnabled = true;
	bUpdated = true;
}

TEffect::~TEffect()
{
	Remove();
}

// +--------------------------------------------------+
// |                  Public Methods                  |
// +--------------------------------------------------+

bool TEffect::IsValid()
{
	lock_guard<mutex> Lock(MutexLock);

	if (eType == BASS_FX_UNKNOWN) return false;
	if (IsRemoved()) return false;

	return true;
}

void TEffect::Reset()
{
	lock_guard<mutex> Lock(MutexLock);
	ResetInternal();
}

bool TEffect::Apply()
{
	lock_guard<mutex> Lock(MutexLock);
	if (pChannel == BASS_NULL) return false;

	return ApplyInternal(pChannel);
}

bool TEffect::Apply(bass_p pChannel)
{
	lock_guard<mutex> Lock(MutexLock);
	return ApplyInternal(pChannel);
}

void TEffect::Update()
{
	lock_guard<mutex> Lock(MutexLock);
	UpdateInternal();
}

void TEffect::Remove()
{
	lock_guard<mutex> Lock(MutexLock);

	RemoveFX();
	pChannel = BASS_NULL;
	bIsRemoved = true;
}

const int TEffect::GetError() const
{
	return iErrorCode;
}

void TEffect::SetEnabled(bool bEnabled)
{
	lock_guard<mutex> Lock(MutexLock);

	bool bChanged = this->bEnabled != bEnabled;
	this->bEnabled = bEnabled;

	if (pChannel == BASS_NULL) return;
	if (!bChanged) return;

	ApplyInternal(pChannel);
}

const bool TEffect::GetEnabled() const
{
	return this->bEnabled;
}

string TEffect::ToString()
{
	return string(*this);
}

TEffect::operator string()
{
	stringstream out;

	out << (*this);

	return string(out.str());
}

ostream& operator<<(ostream& os, TEffect& FX)
{
	os << TEffect::LUAMETANAME << ": " << &FX;
	return os;
}


bass_flag TEffectTrait_Waveform_TS::GetWaveform()
{
	return this->eWaveform.load();
}

void TEffectTrait_Waveform_TS::SetWaveform(bass_flag eWaveform)
{
	switch (eWaveform)
	{
		case BASS_WAVEFORM_TRIANGLE:
		case BASS_WAVEFORM_SINE: this->eWaveform.store(eWaveform); break;

		default: this->eWaveform.store(this->eWaveformDefault);
	}
}

void TEffectTrait_Waveform_TQ::SetWaveform(bass_flag eWaveform)
{
	switch (eWaveform)
	{
		case BASS_WAVEFORM_TRIANGLE:
		case BASS_WAVEFORM_SQUARE: this->eWaveform.store(eWaveform); break;

		default: this->eWaveform.store(this->eWaveformDefault);
	}
}

bass_flag TEffectTrait_Phase::GetPhase()
{
	return this->ePhase.load();
}

void TEffectTrait_Phase::SetPhase(bass_flag ePhase)
{
	switch (ePhase)
	{
		case BASS_DX8_PHASE_NEG_180:
		case BASS_DX8_PHASE_NEG_90:
		case BASS_DX8_PHASE_ZERO:
		case BASS_DX8_PHASE_90:
		case BASS_DX8_PHASE_180: this->ePhase.store(ePhase); break;

		default: this->ePhase.store(this->ePhaseDefault);
	}
}

bass_flag TEffectTrait_Curve::GetCurve()
{
	return this->eCurve.load();
}

void TEffectTrait_Curve::SetCurve(bass_flag eWaveform)
{
	switch (eWaveform)
	{
		case BASS_CURVE_LINEAR:
		case BASS_CURVE_LOGARITHMIC: this->eCurve.store(eWaveform); break;

		default: this->eCurve.store(this->eCurveDefault);
	}
}
