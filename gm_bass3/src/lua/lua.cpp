#include "lua.hpp"

namespace LUAINTERFACE
{
	bool g_ThinkError = false;

	namespace _G {
		TLTCBass* g_BASS3 = NULL;
	}

	namespace _R {
		TLOCChannel* g_IBASS3Channel = NULL;
		TLOCEffectChorus* g_IBASS3FxChorus = NULL;
		TLOCEffectCompressor* g_IBASS3FxCompressor = NULL;
		TLOCEffectDistortion* g_IBASS3FxDistortion = NULL;
		TLOCEffectEcho* g_IBASS3FxEcho = NULL;
		TLOCEffectFlanger* g_IBASS3FxFlanger = NULL;
		TLOCEffectGargle* g_IBASS3FxGargle = NULL;
		TLOCEffectI3DL2Reverb* g_IBASS3FxI3DL2Reverb = NULL;
		TLOCEffectParamEQ* g_IBASS3FxParamEQ = NULL;
		TLOCEffectReverb* g_IBASS3FxReverb = NULL;
		TLOCEffectVolume* g_IBASS3FxVolume = NULL;
	}

	ILuaBase* GetLuaBase(lua_State* state)
	{
		if (ISNULLPTR(state)) return NULL;

		ILuaBase* pLUA = state->luabase;
		if (ISNULLPTR(pLUA)) return NULL;

		pLUA->SetState(state);

		return pLUA;
	}

	bool IsNil(ILuaBase* pLUA, int iStackPos)
	{
		if (pLUA->IsType(iStackPos, Type::NIL)) return true;
		if (pLUA->IsType(iStackPos, Type::NONE)) return true;

		return false;
	}

	void PushVector(ILuaBase* pLUA, BASS_3DVECTOR* pBassVector)
	{
		if (ISNULLPTR(pBassVector))
		{
			pLUA->PushNil();
			return;
		}

		Vector vPos;
		vPos.x = pBassVector->x;
		vPos.y = pBassVector->y;
		vPos.z = pBassVector->z;
		pLUA->PushVector(vPos);
	}

	void SetupRealm(ILuaBase* pLUA)
	{
		// Is it a client?
		pLUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
			pLUA->GetField(-1, "CLIENT");
			g_CLIENT = pLUA->GetBool(-1);
		pLUA->Pop();

		if(g_CLIENT)
		{
			g_IsDedicatedServer = false;
			return;
		}
		pLUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
			pLUA->GetField(-1, "game");
				pLUA->GetField(-1, "IsDedicated");
				pLUA->Call(0, 1);
				g_IsDedicatedServer = pLUA->GetBool(-1);
		pLUA->Pop(2);
	}

	void Init(ILuaBase* pLUA)
	{
		_G::g_BASS3 = new TLTCBass();
		_G::g_BASS3->Init(pLUA);

		_R::g_IBASS3Channel = new TLOCChannel();
		_R::g_IBASS3Channel->Init(pLUA);

		if (g_CLIENT)
		{
			_R::g_IBASS3FxChorus = new TLOCEffectChorus();
			_R::g_IBASS3FxChorus->Init(pLUA);

			_R::g_IBASS3FxCompressor = new TLOCEffectCompressor();
			_R::g_IBASS3FxCompressor->Init(pLUA);

			_R::g_IBASS3FxDistortion = new TLOCEffectDistortion();
			_R::g_IBASS3FxDistortion->Init(pLUA);

			_R::g_IBASS3FxEcho = new TLOCEffectEcho();
			_R::g_IBASS3FxEcho->Init(pLUA);

			_R::g_IBASS3FxFlanger = new TLOCEffectFlanger();
			_R::g_IBASS3FxFlanger->Init(pLUA);

			_R::g_IBASS3FxGargle = new TLOCEffectGargle();
			_R::g_IBASS3FxGargle->Init(pLUA);

			_R::g_IBASS3FxI3DL2Reverb = new TLOCEffectI3DL2Reverb();
			_R::g_IBASS3FxI3DL2Reverb->Init(pLUA);

			_R::g_IBASS3FxParamEQ = new TLOCEffectParamEQ();
			_R::g_IBASS3FxParamEQ->Init(pLUA);

			_R::g_IBASS3FxReverb = new TLOCEffectReverb();
			_R::g_IBASS3FxReverb->Init(pLUA);

			_R::g_IBASS3FxVolume = new TLOCEffectVolume();
			_R::g_IBASS3FxVolume->Init(pLUA);
		}

		// Add pulling hook
		pLUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		pLUA->GetField(-1, "hook");
		pLUA->GetField(-1, "Add");

		if (g_CLIENT)
		{
			// This does not get paused in single player
			pLUA->PushString("PostRenderVGUI");
		}
		else
		{
			pLUA->PushString("Think");
		}

		pLUA->PushString("IBASS3_PULL");
		pLUA->PushCFunction([](lua_State* state) -> int {
			ILuaBase* pLUA = LUAINTERFACE::GetLuaBase(state);
			if (ISNULLPTR(pLUA)) return 0;
			if (LUAINTERFACE::g_ThinkError) return 0;

			if (g_CLOSING) return 0;
			g_IntialTickHappend = true;

			try {
				for (const auto &pair : TLuaTableContainer::g_mapTablesInstances) {
					auto name = pair.first;
					auto instance = pair.second;
					if (ISNULLPTR(instance)) continue;

					instance->Think(pLUA);
				}
				
				for (const auto &pair : TLuaObjectContainer::g_mapObjectInstances) {
					auto name = pair.first;
					auto instance = pair.second;
					if (ISNULLPTR(instance)) continue;

					instance->Think(pLUA);
				}
			}
			catch (const overflow_error& e)
			{
				LUAINTERFACE::g_ThinkError = true;
				snprintf(g_pcErrorBuffer, ERROR_PUFFER_SIZE, "IBASS3_PULL failed, overflow_error exception: %s\n", e.what());
				pLUA->ThrowError(g_pcErrorBuffer);

				return 0;
			}
			catch (const runtime_error& e)
			{
				LUAINTERFACE::g_ThinkError = true;
				snprintf(g_pcErrorBuffer, ERROR_PUFFER_SIZE, "IBASS3_PULL failed, runtime_error exception: %s\n", e.what());
				pLUA->ThrowError(g_pcErrorBuffer);

				return 0;
			}
			catch (const exception& e)
			{
				LUAINTERFACE::g_ThinkError = true;
				snprintf(g_pcErrorBuffer, ERROR_PUFFER_SIZE, "IBASS3_PULL failed, exception: %s\n", e.what());
				pLUA->ThrowError(g_pcErrorBuffer);

				return 0;
			}
			catch (string s)
			{
				LUAINTERFACE::g_ThinkError = true;
				snprintf(g_pcErrorBuffer, ERROR_PUFFER_SIZE, "IBASS3_PULL failed, exception: %s\n", s.c_str());
				pLUA->ThrowError(g_pcErrorBuffer);

				return 0;
			}
			catch (char* s)
			{
				LUAINTERFACE::g_ThinkError = true;
				snprintf(g_pcErrorBuffer, ERROR_PUFFER_SIZE, "IBASS3_PULL failed, exception: %s\n", s);
				pLUA->ThrowError(g_pcErrorBuffer);

				return 0;
			}
			catch (...)
			{
				g_ThinkError = true;
				pLUA->ThrowError("IBASS3_PULL failed, unknown exception.\n");
				return 0;
			}

			LUAINTERFACE::g_ThinkError = false;
			return 0;
		});

		pLUA->Call(3, 0);
		pLUA->Pop(2);
	}

	void Dispose(ILuaBase* pLUA)
	{
		for (const auto &pair : TLuaTableContainer::g_mapTablesInstances) {
			auto name = pair.first;
			auto instance = pair.second;
			if (ISNULLPTR(instance)) continue;

			instance->Dispose(pLUA);
			delete instance;
		}

		for (const auto &pair : TLuaObjectContainer::g_mapObjectInstances) {
			auto name = pair.first;
			auto instance = pair.second;
			if (ISNULLPTR(instance)) continue;

			instance->Dispose(pLUA);
			delete instance;
		}

		_G::g_BASS3 = NULL;
		_R::g_IBASS3Channel = NULL;
		_R::g_IBASS3FxChorus = NULL;
		_R::g_IBASS3FxCompressor = NULL;
		_R::g_IBASS3FxDistortion = NULL;
		_R::g_IBASS3FxEcho = NULL;
		_R::g_IBASS3FxFlanger = NULL;
		_R::g_IBASS3FxGargle = NULL;
		_R::g_IBASS3FxI3DL2Reverb = NULL;
		_R::g_IBASS3FxParamEQ = NULL;
		_R::g_IBASS3FxReverb = NULL;
		_R::g_IBASS3FxVolume = NULL;
	}
}