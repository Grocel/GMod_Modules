#include "teffectcompressor.hpp"

string TEffectCompressor::LUAMETANAME = "IBASS3FxCompressor";
int TEffectCompressor::LUAMETAID = 0;

void* TEffectCompressor::GetData()
{
	if (ISNULLPTR(pData))
	{
		pData = new BASS_DX8_COMPRESSOR();
	}

	if (ISNULLPTR(pData))
	{
		return NULL;
	}

	pData->fGain = this->GetGain();
	pData->fAttack = this->GetAttack();
	pData->fRelease = this->GetRelease();
	pData->fThreshold = this->GetThreshold();
	pData->fRatio = this->GetRatio();
	pData->fPredelay = this->GetPredelay();

	return pData;
}

TEffectCompressor::TEffectCompressor()
{
	pData = NULL;
	eType = BASS_FX_DX8_COMPRESSOR;
}

TEffectCompressor::~TEffectCompressor()
{
	if (ISNULLPTR(pData)) return;
	delete pData;
}

string TEffectCompressor::ToString()
{
	return string(*this);
}

TEffectCompressor::operator string()
{
	stringstream out;

	out << (*this);

	return string(out.str());
}

ostream& operator<<(ostream& os, TEffectCompressor& FX)
{
	if (!FX.IsValid())
	{
		os << "[NULL " << TEffectCompressor::LUAMETANAME << "]";
		return os;
	}

	os << TEffectCompressor::LUAMETANAME << ": " << &FX;
	return os;
}
