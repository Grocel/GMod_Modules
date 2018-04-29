#include "tluaobjectcontainer.hpp"

#ifndef T_LOCEFFECT_BASE_H
#define T_LOCEFFECT_BASE_H

namespace LUAINTERFACE
{
	class TLOCEffectBase : public TLuaObjectContainer
	{
	protected:
		template<class T> void AddEffectBaseMethods(ILuaBase* pLUA)
		{
			this->AddMethod("__gc", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				LUAINTERFACE::HELPER::Check<T>(pLUA, 1);
				T* pOBJ = LUAINTERFACE::HELPER::Get<T>(pLUA, 1);

				LUAINTERFACE::HELPER::Delete<T>(pOBJ, true);
				return 0;
			});

			this->AddMethod("GetTypeEnum", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				pLUA->PushNumber(LUAENUM_FX_UNKNOWN);
				return 1;
			});

			this->AddMethod("GetError", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				LUAINTERFACE::HELPER::Check<T>(pLUA, 1);
				T* pOBJ = LUAINTERFACE::HELPER::Get<T>(pLUA, 1);
				if (ISNULLPTR(pOBJ)) return 0;

				pLUA->PushNumber(pOBJ->GetError());
				return 1;
			});

			LUA_FUNCTION_GETSET_BOOL(T, Enabled);
		}
	};

	namespace TLOCEffectTrait_Waveform_TS
	{
		template<class T> void AddMethods(TLuaObjectContainer* thisOBJ, ILuaBase* pLUA)
		{
			thisOBJ->AddMethod("GetWaveform", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				T* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);

				bass_flag eReturn = pOBJ->GetWaveform();
				bass_flag eLuaReturn = BASS_NULL;

				switch (eLuaReturn)
				{
					case BASS_WAVEFORM_TRIANGLE: eLuaReturn = LUAENUM_FXPARAMETER_WAVEFORM_TRIANGLE; break;
					case BASS_WAVEFORM_SINE: eLuaReturn = LUAENUM_FXPARAMETER_WAVEFORM_SINE; break;
					default: return 0;
				}

				pLUA->PushNumber(eLuaReturn);
				return 1;
			});

			thisOBJ->AddMethod("SetWaveform", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				T* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
				pLUA->CheckType(2, Type::NUMBER);

				bass_flag eLuaInput = (bass_flag)pLUA->GetNumber(2);
				bass_flag eInput = BASS_NULL;

				switch (eLuaInput)
				{
					case LUAENUM_FXPARAMETER_WAVEFORM_SINE: eInput = BASS_WAVEFORM_SINE; break;
					case LUAENUM_FXPARAMETER_WAVEFORM_TRIANGLE: eInput = BASS_WAVEFORM_TRIANGLE; break;
					case LUAENUM_FXPARAMETER_WAVEFORM_SQUARE: pLUA->ArgError(2, "The FXPARAMETER_WAVEFORM_SQUARE is not supported by this object type.");
					default: pLUA->ArgError(2, "An invalid FXPARAMETER_WAVEFORM_* ENUM was given.");
				}

				pOBJ->SetWaveform(eInput);
				return 0;
			});
		}
	};

	namespace TLOCEffectTrait_Waveform_TQ
	{
		template<class T> void AddMethods(TLuaObjectContainer* thisOBJ, ILuaBase* pLUA)
		{
			thisOBJ->AddMethod("GetWaveform", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				T* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);

				bass_flag eReturn = pOBJ->GetWaveform();
				bass_flag eLuaReturn = BASS_NULL;

				switch (eLuaReturn)
				{
					case BASS_WAVEFORM_TRIANGLE: eLuaReturn = LUAENUM_FXPARAMETER_WAVEFORM_TRIANGLE; break;
					case BASS_WAVEFORM_SQUARE: eLuaReturn = LUAENUM_FXPARAMETER_WAVEFORM_SQUARE; break;
					default: return 0;
				}

				pLUA->PushNumber(eLuaReturn);
				return 1;
			});

			thisOBJ->AddMethod("SetWaveform", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				T* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
				pLUA->CheckType(2, Type::NUMBER);

				bass_flag eLuaInput = (bass_flag)pLUA->GetNumber(2);
				bass_flag eInput = BASS_NULL;

				switch (eLuaInput)
				{
					case LUAENUM_FXPARAMETER_WAVEFORM_SQUARE: eInput = BASS_WAVEFORM_SQUARE; break;
					case LUAENUM_FXPARAMETER_WAVEFORM_TRIANGLE: eInput = BASS_WAVEFORM_TRIANGLE; break;
					case LUAENUM_FXPARAMETER_WAVEFORM_SINE: pLUA->ArgError(2, "The LUAENUM_FXPARAMETER_WAVEFORM_SINE is not supported by this object type.");
					default: pLUA->ArgError(2, "An invalid FXPARAMETER_WAVEFORM_* ENUM was given.");
				}

				pOBJ->SetWaveform(eInput);
				return 0;
			});

		}
	};


	namespace TLOCEffectTrait_Phase
	{
		template<class T> void AddMethods(TLuaObjectContainer* thisOBJ, ILuaBase* pLUA)
		{
			thisOBJ->AddMethod("GetPhase", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				T* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);

				bass_flag eReturn = pOBJ->GetPhase();
				bass_flag eLuaReturn = BASS_NULL;

				switch (eReturn)
				{
					case BASS_DX8_PHASE_NEG_180: eLuaReturn = LUAENUM_FXPARAMETER_PHASE_N180; break;
					case BASS_DX8_PHASE_NEG_90: eLuaReturn = LUAENUM_FXPARAMETER_PHASE_N90; break;
					case BASS_DX8_PHASE_ZERO: eLuaReturn = LUAENUM_FXPARAMETER_PHASE_ZERO; break;
					case BASS_DX8_PHASE_90: eLuaReturn = LUAENUM_FXPARAMETER_PHASE_P90; break;
					case BASS_DX8_PHASE_180: eLuaReturn = LUAENUM_FXPARAMETER_PHASE_P180; break;
					default: return 0;
				}

				pLUA->PushNumber(eLuaReturn);
				return 1;
			});

			thisOBJ->AddMethod("SetPhase", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				T* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
				pLUA->CheckType(2, Type::NUMBER);

				bass_flag eLuaInput = (bass_flag)pLUA->GetNumber(2);
				bass_flag eInput = BASS_NULL;

				switch (eLuaInput)
				{
					case LUAENUM_FXPARAMETER_PHASE_N180: eInput = BASS_DX8_PHASE_NEG_180; break;
					case LUAENUM_FXPARAMETER_PHASE_N90: eInput = BASS_DX8_PHASE_NEG_90; break;
					case LUAENUM_FXPARAMETER_PHASE_ZERO: eInput = BASS_DX8_PHASE_ZERO; break;
					case LUAENUM_FXPARAMETER_PHASE_P90: eInput = BASS_DX8_PHASE_90; break;
					case LUAENUM_FXPARAMETER_PHASE_P180: eInput = BASS_DX8_PHASE_180; break;
					default: pLUA->ArgError(2, "An invalid FXPARAMETER_PHASE_* ENUM was given.");
				}

				pOBJ->SetPhase(eInput);
				return 0;
			});
		}
	};

	namespace TLOCEffectTrait_Curve
	{
		template<class T> void AddMethods(TLuaObjectContainer* thisOBJ, ILuaBase* pLUA)
		{
			thisOBJ->AddMethod("GetCurve", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				T* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);

				bass_flag eReturn = pOBJ->GetCurve();
				bass_flag eLuaReturn = BASS_NULL;

				switch (eReturn)
				{
					case BASS_CURVE_LINEAR: eLuaReturn = LUAENUM_FXPARAMETER_CURVE_LINEAR; break;
					case BASS_CURVE_LOGARITHMIC: eLuaReturn = LUAENUM_FXPARAMETER_CURVE_LOGARITHMIC; break;
					default: return 0;
				}

				pLUA->PushNumber(eLuaReturn);
				return 1;
			});

			thisOBJ->AddMethod("SetCurve", [](lua_State* state) -> int {
				ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
				if (ISNULLPTR(pLUA)) return 0;

				T* pOBJ = LUAINTERFACE::HELPER::GetValid<T>(pLUA, 1);
				pLUA->CheckType(2, Type::NUMBER);

				bass_flag eLuaInput = (bass_flag)pLUA->GetNumber(2);
				bass_flag eInput = BASS_NULL;

				switch (eLuaInput)
				{
					case LUAENUM_FXPARAMETER_CURVE_LINEAR: eInput = BASS_CURVE_LINEAR; break;
					case LUAENUM_FXPARAMETER_CURVE_LOGARITHMIC: eInput = BASS_CURVE_LOGARITHMIC; break;
					default: pLUA->ArgError(2, "An invalid FXPARAMETER_CURVE_* ENUM was given.");
				}

				pOBJ->SetCurve(eInput);
				return 0;
			});

		}
	};

}
#endif