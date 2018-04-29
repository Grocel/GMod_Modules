#include "tloceffectbase.hpp"
#include "../../classes/teffect/teffectchorus.hpp"

#ifndef T_LOCEFFECT_CHORUS_H
#define T_LOCEFFECT_CHORUS_H

namespace LUAINTERFACE
{
	class TLOCEffectChorus : public TLOCEffectBase
	{
	protected:
		template<class T> void AddMethods(ILuaBase* pLUA)
		{
			TLuaObjectContainer::AddBaseMethods<T>(pLUA);
			TLOCEffectBase::AddEffectBaseMethods<T>(pLUA);

			this->AddMethod("GetTypeEnum", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				pLUA->PushNumber(LUAENUM_FX_CHORUS);
				return 1;
			});

			TLOCEffectTrait_Waveform_TS::AddMethods<T>(this, pLUA);
			TLOCEffectTrait_Phase::AddMethods<T>(this, pLUA);

			LUA_FUNCTION_GETSET_NUMBER(T, WetDryMix, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Depth, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Feedback, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Frequency, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Delay, float);
		}

	public:

		template<class T> void Init(ILuaBase* pLUA)
		{
			AddMethods<T>(pLUA);
			T::LUAMETAID = Build(pLUA, T::LUAMETANAME);
		}

		void Init(ILuaBase* pLUA)
		{
			Init<TEffectChorus>(pLUA);
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