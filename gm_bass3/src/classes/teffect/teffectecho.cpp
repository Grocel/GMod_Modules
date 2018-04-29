#include "teffectecho.hpp"

string TEffectEcho::LUAMETANAME = "IBASS3FxEcho";
int TEffectEcho::LUAMETAID = 0;

void* TEffectEcho::GetData()
{
	if (ISNULLPTR(pData))
	{
		pData = new BASS_DX8_ECHO();
	}

	if (ISNULLPTR(pData))
	{
		return NULL;
	}

	pData->fWetDryMix = this->GetWetDryMix();
	pData->fFeedback = this->GetFeedback();
	pData->fLeftDelay = this->GetLeftDelay();
	pData->fRightDelay = this->GetRightDelay();
	pData->lPanDelay = this->lPanDelay;

	return pData;
}

TEffectEcho::TEffectEcho()
{
	pData = NULL;
	eType = BASS_FX_DX8_ECHO;
}

TEffectEcho::~TEffectEcho()
{
	if (ISNULLPTR(pData)) return;
	delete pData;
}

string TEffectEcho::ToString()
{
	return string(*this);
}

TEffectEcho::operator string()
{
	stringstream out;

	out << (*this);

	return string(out.str());
}

ostream& operator<<(ostream& os, TEffectEcho& FX)
{
	if (!FX.IsValid())
	{
		os << "[NULL " << TEffectEcho::LUAMETANAME << "]";
		return os;
	}

	os << TEffectEcho::LUAMETANAME << ": " << &FX;
	return os;
}
