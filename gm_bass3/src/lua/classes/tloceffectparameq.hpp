#include "tloceffectbase.hpp"
#include "../../classes/teffect/teffectparameq.hpp"

#ifndef T_LOCEFFECT_PARAMEQ_H
#define T_LOCEFFECT_PARAMEQ_H

namespace LUAINTERFACE
{
	class TLOCEffectParamEQ : public TLOCEffectBase
	{
	protected:
		template<class T> void AddMethods(ILuaBase* pLUA)
		{
			TLuaObjectContainer::AddBaseMethods<T>(pLUA);
			TLOCEffectBase::AddEffectBaseMethods<T>(pLUA);

			this->AddMethod("GetTypeEnum", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				pLUA->PushNumber(LUAENUM_FX_PARAMEQ);
				return 1;
			});

			LUA_FUNCTION_GETSET_NUMBER(T, Center, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Bandwidth, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Gain, float);
		}

	public:

		template<class T> void Init(ILuaBase* pLUA)
		{
			AddMethods<T>(pLUA);
			T::LUAMETAID = Build(pLUA, T::LUAMETANAME);
		}

		void Init(ILuaBase* pLUA)
		{
			Init<TEffectParamEQ>(pLUA);
		}

		void Think(ILuaBase* pLUA)
		{
		}

		void Dispose(ILuaBase* pLUA)
		{
		}
	};
}
#endif