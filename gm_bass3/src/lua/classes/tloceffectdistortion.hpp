#include "tloceffectbase.hpp"
#include "../../classes/teffect/teffectdistortion.hpp"

#ifndef T_LOCEFFECT_DISTORTION_H
#define T_LOCEFFECT_DISTORTION_H

namespace LUAINTERFACE
{
	class TLOCEffectDistortion : public TLOCEffectBase
	{
	protected:
		template<class T> void AddMethods(ILuaBase* pLUA)
		{
			TLuaObjectContainer::AddBaseMethods<T>(pLUA);
			TLOCEffectBase::AddEffectBaseMethods<T>(pLUA);

			this->AddMethod("GetTypeEnum", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				pLUA->PushNumber(LUAENUM_FX_DISTORTION);
				return 1;
			});

			LUA_FUNCTION_GETSET_NUMBER(T, Gain, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Edge, float);
			LUA_FUNCTION_GETSET_NUMBER(T, PostEQCenterFrequency, float);
			LUA_FUNCTION_GETSET_NUMBER(T, PostEQBandwidth, float);
			LUA_FUNCTION_GETSET_NUMBER(T, PreLowpassCutoff, float);
		}

	public:

		template<class T> void Init(ILuaBase* pLUA)
		{
			AddMethods<T>(pLUA);
			T::LUAMETAID = Build(pLUA, T::LUAMETANAME);
		}

		void Init(ILuaBase* pLUA)
		{
			Init<TEffectDistortion>(pLUA);
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