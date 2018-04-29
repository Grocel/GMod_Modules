#include "tloceffectbase.hpp"
#include "../../classes/teffect/teffecti3dl2reverb.hpp"

#ifndef T_LOCEFFECT_I3DL2REVERB_H
#define T_LOCEFFECT_I3DL2REVERB_H

namespace LUAINTERFACE
{
	class TLOCEffectI3DL2Reverb : public TLOCEffectBase
	{
	protected:
		template<class T> void AddMethods(ILuaBase* pLUA)
		{
			TLuaObjectContainer::AddBaseMethods<T>(pLUA);
			TLOCEffectBase::AddEffectBaseMethods<T>(pLUA);

			this->AddMethod("GetTypeEnum", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				pLUA->PushNumber(LUAENUM_FX_I3DL2REVERB);
				return 1;
			});

			LUA_FUNCTION_GETSET_NUMBER(T, Room, int);
			LUA_FUNCTION_GETSET_NUMBER(T, RoomHF, int);
			LUA_FUNCTION_GETSET_NUMBER(T, RoomRolloffFactor, float);
			LUA_FUNCTION_GETSET_NUMBER(T, DecayTime, float);
			LUA_FUNCTION_GETSET_NUMBER(T, DecayHFRatio, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Reflections, int);
			LUA_FUNCTION_GETSET_NUMBER(T, ReflectionsDelay, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Reverb, int);
			LUA_FUNCTION_GETSET_NUMBER(T, ReverbDelay, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Diffusion, float);
			LUA_FUNCTION_GETSET_NUMBER(T, Density, float);
			LUA_FUNCTION_GETSET_NUMBER(T, HFReference, float);
		}

	public:

		template<class T> void Init(ILuaBase* pLUA)
		{
			AddMethods<T>(pLUA);
			T::LUAMETAID = Build(pLUA, T::LUAMETANAME);
		}

		void Init(ILuaBase* pLUA)
		{
			Init<TEffectI3DL2Reverb>(pLUA);
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