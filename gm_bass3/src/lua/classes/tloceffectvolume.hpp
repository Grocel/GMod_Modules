#include "tloceffectbase.hpp"
#include "../../classes/teffect/teffectvolume.hpp"

#ifndef T_LOCEFFECT_VOLUME_H
#define T_LOCEFFECT_VOLUME_H

namespace LUAINTERFACE
{
	class TLOCEffectVolume : public TLOCEffectBase
	{
	protected:
		template<class T> void AddMethods(ILuaBase* pLUA)
		{
			TLuaObjectContainer::AddBaseMethods<T>(pLUA);
			TLOCEffectBase::AddEffectBaseMethods<T>(pLUA);

			this->AddMethod("GetTypeEnum", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				pLUA->PushNumber(LUAENUM_FX_VOLUME);
				return 1;
			});

			TLOCEffectTrait_Curve::AddMethods<T>(this, pLUA);

			LUA_FUNCTION_GETSET_NUMBER(T, Target, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Current, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Time, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Curve, bass_flag);
		}

	public:

		template<class T> void Init(ILuaBase* pLUA)
		{
			AddMethods<T>(pLUA);
			T::LUAMETAID = Build(pLUA, T::LUAMETANAME);
		}

		void Init(ILuaBase* pLUA)
		{
			Init<TEffectVolume>(pLUA);
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