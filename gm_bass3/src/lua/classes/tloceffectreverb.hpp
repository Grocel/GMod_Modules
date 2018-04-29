#include "tloceffectbase.hpp"
#include "../../classes/teffect/teffectreverb.hpp"

#ifndef T_LOCEFFECT_REVERB_H
#define T_LOCEFFECT_REVERB_H

namespace LUAINTERFACE
{
	class TLOCEffectReverb : public TLOCEffectBase
	{
	protected:
		template<class T> void AddMethods(ILuaBase* pLUA)
		{
			TLuaObjectContainer::AddBaseMethods<T>(pLUA);
			TLOCEffectBase::AddEffectBaseMethods<T>(pLUA);

			this->AddMethod("GetTypeEnum", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				pLUA->PushNumber(LUAENUM_FX_REVERB);
				return 1;
			});

			LUA_FUNCTION_GETSET_NUMBER(T, InGain, float);
			LUA_FUNCTION_GETSET_NUMBER(T, ReverbMix, float);
			LUA_FUNCTION_GETSET_NUMBER(T, ReverbTime, float);
			LUA_FUNCTION_GETSET_NUMBER(T, HighFreqRTRatio, float);
		}

	public:

		template<class T> void Init(ILuaBase* pLUA)
		{
			AddMethods<T>(pLUA);
			T::LUAMETAID = Build(pLUA, T::LUAMETANAME);
		}

		void Init(ILuaBase* pLUA)
		{
			Init<TEffectReverb>(pLUA);
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