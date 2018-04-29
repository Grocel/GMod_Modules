#include "tloceffectbase.hpp"
#include "../../classes/teffect/teffectgargle.hpp"

#ifndef T_LOCEFFECT_GARGLE_H
#define T_LOCEFFECT_GARGLE_H

namespace LUAINTERFACE
{
	class TLOCEffectGargle : public TLOCEffectBase
	{
	protected:
		template<class T> void AddMethods(ILuaBase* pLUA)
		{
			TLuaObjectContainer::AddBaseMethods<T>(pLUA);
			TLOCEffectBase::AddEffectBaseMethods<T>(pLUA);

			this->AddMethod("GetTypeEnum", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				pLUA->PushNumber(LUAENUM_FX_GARGLE);
				return 1;
			});

			TLOCEffectTrait_Waveform_TQ::AddMethods<T>(this, pLUA);

			this->AddMethod("SetRate", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				T* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
				pLUA->CheckType(2, Type::NUMBER);

				double fRate = pLUA->GetNumber(2);
				if (fRate < 0) {
					fRate = 0;
				}

				pOBJ->SetRate(fRate);
				return 0;
			});

			LUA_FUNCTION_GETRAW_NUMBER(T, GetRate, DWORD);
		}

	public:

		template<class T> void Init(ILuaBase* pLUA)
		{
			AddMethods<T>(pLUA);
			T::LUAMETAID = Build(pLUA, T::LUAMETANAME);
		}

		void Init(ILuaBase* pLUA)
		{
			Init<TEffectGargle>(pLUA);
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