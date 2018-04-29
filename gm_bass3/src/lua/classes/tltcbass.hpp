#include "tluatablecontainer.hpp"

#ifndef T_LTCBASS_H
#define T_LTCBASS_H

namespace LUAINTERFACE
{
	class TLTCBass : public TLuaTableContainer
	{
	public:
		TLTCBass();

		void Init(ILuaBase* pLUA);
		void Think(ILuaBase* pLUA);
		void Dispose(ILuaBase* pLUA);
	};
}
#endif