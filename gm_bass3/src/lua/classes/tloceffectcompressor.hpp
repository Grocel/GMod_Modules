#include "tloceffectbase.hpp"
#include "../../classes/teffect/teffectcompressor.hpp"

#ifndef T_LOCEFFECT_COMPRESSOR_H
#define T_LOCEFFECT_COMPRESSOR_H

namespace LUAINTERFACE
{
	class TLOCEffectCompressor : public TLOCEffectBase
	{
	protected:
		template<class T> void AddMethods(ILuaBase* pLUA)
		{
			TLuaObjectContainer::AddBaseMethods<T>(pLUA);
			TLOCEffectBase::AddEffectBaseMethods<T>(pLUA);

			this->AddMethod("GetTypeEnum", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				pLUA->PushNumber(LUAENUM_FX_COMPRESSOR);
				return 1;
			});

			LUA_FUNCTION_GETSET_NUMBER(T, Gain, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Attack, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Release, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Threshold, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Ratio, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Predelay, float);
		}

	public:

		template<class T> void Init(ILuaBase* pLUA)
		{
			AddMethods<T>(pLUA);
			T::LUAMETAID = Build(pLUA, T::LUAMETANAME);
		}

		void Init(ILuaBase* pLUA)
		{
			Init<TEffectCompressor>(pLUA);
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