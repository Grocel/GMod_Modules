#include "tluarefable.hpp"

string TLuaRefAble::LUAMETANAME = "IBASS3GenericObject";
int TLuaRefAble::LUAMETAID = 0;
unsigned long long TLuaRefAble::g_id = 0;


// +--------------------------------------------------+
// |              Constructor/Destructor              |
// +--------------------------------------------------+

TLuaRefAble::TLuaRefAble()
{
	TLuaRefAble::g_id++;
	id = TLuaRefAble::g_id;

	iReferences = 0;
	AddReference();
}

TLuaRefAble::~TLuaRefAble()
{
	Remove();
}

// +--------------------------------------------------+
// |                  Public Methods                  |
// +--------------------------------------------------+

bool TLuaRefAble::IsValid()
{
	return !bIsRemoved;
}

void TLuaRefAble::Remove()
{
	bIsRemoved = true;
}

const bool TLuaRefAble::IsRemoved() const
{
	return bIsRemoved;
}

unsigned int TLuaRefAble::AddReference()
{
	lock_guard<mutex> Lock(MutexLock);
	iReferences++;
	return iReferences;
}

unsigned int TLuaRefAble::RemoveReference()
{
	lock_guard<mutex> Lock(MutexLock);
	if (iReferences) iReferences--;
	return iReferences;
}

const unsigned int TLuaRefAble::GetReferences() const
{
	return iReferences;
}

const bool TLuaRefAble::IsReferenced() const
{
	return iReferences > 0;
}


string TLuaRefAble::ToString()
{
	return string(*this);
}

TLuaRefAble::operator string()
{
	stringstream out;

	out << (*this);

	return string(out.str());
}

bool TLuaRefAble::operator ==(TLuaRefAble& other)
{
	return this == &other;
}

// +--------------------------------------------------+
// |                    Friends                       |
// +--------------------------------------------------+

ostream& operator<<(ostream& os, TLuaRefAble& OBJ)
{
	os << TLuaRefAble::LUAMETANAME << ": " << &OBJ;
	return os;
}
