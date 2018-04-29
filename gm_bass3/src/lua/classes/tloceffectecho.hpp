#include "tloceffectbase.hpp"
#include "../../classes/teffect/teffectecho.hpp"

#ifndef T_LOCEFFECT_ECHO_H
#define T_LOCEFFECT_ECHO_H

namespace LUAINTERFACE
{
	class TLOCEffectEcho : public TLOCEffectBase
	{
	protected:
		template<class T> void AddMethods(ILuaBase* pLUA)
		{
			TLuaObjectContainer::AddBaseMethods<T>(pLUA);
			TLOCEffectBase::AddEffectBaseMethods<T>(pLUA);

			this->AddMethod("GetTypeEnum", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				pLUA->PushNumber(LUAENUM_FX_ECHO);
				return 1;
			});

			LUA_FUNCTION_GETSET_NUMBER(T, WetDryMix, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Feedback, float);
			LUA_FUNCTION_GETSET_NUMBER(T, LeftDelay, float);
			LUA_FUNCTION_GETSET_NUMBER(T, RightDelay, float);
			LUA_FUNCTION_GETSET_BOOL(T, PanDelay);
		}

	public:

		template<class T> void Init(ILuaBase* pLUA)
		{
			AddMethods<T>(pLUA);
			T::LUAMETAID = Build(pLUA, T::LUAMETANAME);
		}

		void Init(ILuaBase* pLUA)
		{
			Init<TEffectEcho>(pLUA);
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