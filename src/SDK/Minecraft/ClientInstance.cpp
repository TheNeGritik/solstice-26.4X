//
// Created by vastrakai on 6/24/2024.
//

#include "ClientInstance.hpp"

#include <libhat/Access.hpp>
#include <SDK/OffsetProvider.hpp>
#include <SDK/SigManager.hpp>

#include "MinecraftGame.hpp"

#include <Utils/Structs.hpp>

#include "Actor/Actor.hpp"

#include "Options.hpp"
#include "KeyboardRemappingLayout.hpp"
#include "Rendering/LevelRenderer.hpp"
#include "Rendering/GuiData.hpp"
#include <Hook/Hooks/RenderHooks/LevelRendererHook.hpp>
#include <Features/FeatureManager.hpp>
#include <Features/Modules/Visual/ClickGui.hpp>
#include <Features/Modules/Visual/HudEditor.hpp>
#include <Windows.h>
#include <cmath>

static bool safeCopyMatrix(const void* src, void* dst)
{
	if (!src || !dst) return false;
	__try {
		memcpy(dst, src, sizeof(float) * 16);
		return true;
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		return false;
	}
}

static bool safeReadFloat(const void* src, float* dst)
{
	if (!src || !dst) return false;
	__try {
		*dst = *reinterpret_cast<const float*>(src);
		return true;
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		return false;
	}
}

ClientInstance* ClientInstance::get()
{
	MinecraftGame* game = MinecraftGame::getInstance();
	if (game == nullptr || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(game)))
		return nullptr;

	static ClientInstance* instance = nullptr;
	if (!instance || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(instance))) {
		instance = game->getPrimaryClientInstance();
	} else {
		ClientInstance* current = game->getPrimaryClientInstance();
		if (current && MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(current))) {
			instance = current;
		}
	}
	return (instance && MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(instance))) ? instance : nullptr;
}

glmatrixf ClientInstance::getViewMatrix()
{
	static glmatrixf sLastViewMatrix = glm::mat4(1.0f);
	if (!this || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(this))) return sLastViewMatrix;

	auto lr = getLevelRenderer();
	if (lr && MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(lr))) {
		auto lrPlayer = lr->getRendererPlayer();
		if (lrPlayer && MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(lrPlayer))) {
			glmatrixf* mat = lrPlayer->getViewMatrix();
			if (mat && MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(mat))) {
				glmatrixf temp;
				if (safeCopyMatrix(mat, &temp)) {
					if (temp[3][3] != 0.0f || temp[0][0] != 0.0f) {
						sLastViewMatrix = glm::transpose(temp);
						return sLastViewMatrix;
					}
				}
			}
		}
	}

	glmatrixf memberMat;
	const void* memberPtr = reinterpret_cast<const char*>(this) + OffsetProvider::ClientInstance_mViewMatrix;
	if (safeCopyMatrix(memberPtr, &memberMat)) {
		if (memberMat[3][3] != 0.0f || memberMat[0][0] != 0.0f) {
			sLastViewMatrix = memberMat;
			return sLastViewMatrix;
		}
	}

	return sLastViewMatrix;
}

glm::vec2 ClientInstance::getFov()
{
	static glm::vec2 sLastFov = { 1.0f, 1.0f };
	if (!this || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(this))) return sLastFov;

	// 1. Try LevelRendererPlayer FOV if valid
	auto lr = getLevelRenderer();
	if (lr && MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(lr))) {
		auto lrPlayer = lr->getRendererPlayer();
		if (lrPlayer && MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(lrPlayer))) {
			float* xPtr = lrPlayer->getFovX();
			float* yPtr = lrPlayer->getFovY();
			float valX = 0.0f, valY = 0.0f;
			if (safeReadFloat(xPtr, &valX) && safeReadFloat(yPtr, &valY)) {
				if (valX > 0.05f && valX < 10.0f && valY > 0.05f && valY < 10.0f && !std::isnan(valX) && !std::isnan(valY)) {
					sLastFov = { valX, valY };
					return sLastFov;
				}
			}
		}
	}

	// 2. Try ClientInstance FOV offsets
	float cx = 0.0f, cy = 0.0f;
	const void* fovXPtr = reinterpret_cast<const char*>(this) + OffsetProvider::ClientInstance_mFovX;
	const void* fovYPtr = reinterpret_cast<const char*>(this) + OffsetProvider::ClientInstance_mFovY;
	if (safeReadFloat(fovXPtr, &cx) && safeReadFloat(fovYPtr, &cy)) {
		if (cx > 0.05f && cx < 10.0f && cy > 0.05f && cy < 10.0f && !std::isnan(cx) && !std::isnan(cy)) {
			sLastFov = { cx, cy };
			return sLastFov;
		}
	}

	// 3. Fallback: compute from standard 70 deg FOV and GuiData aspect ratio
	auto guiData = getGuiData();
	float aspect = (guiData && guiData->mResolution.y > 0) ? (guiData->mResolution.x / guiData->mResolution.y) : (16.0f / 9.0f);
	float fovY = 1.0f / tanf(glm::radians(70.0f) / 2.0f);
	float fovX = fovY / aspect;
	sLastFov = { fovX, fovY };
	return sLastFov;
}

MinecraftSim* ClientInstance::getMinecraftSim()
{
	if (!this || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(this))) return nullptr;

	auto isValidSim = [](MinecraftSim* sim) -> bool {
		if (!sim || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(sim))) return false;
		__try {
			auto game = sim->getGameSim();
			if (!game || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(game))) return false;
			if (game->mTimer >= 0.1f && game->mTimer <= 1000.f &&
				game->mTimerMultiplier >= 0.01f && game->mTimerMultiplier <= 100.f)
			{
				return true;
			}
		} __except (EXCEPTION_EXECUTE_HANDLER) {
			return false;
		}
		return false;
	};

	// 1. Try OffsetProvider offset (0x1A8)
	__try {
		auto sim = hat::member_at<MinecraftSim*>(this, OffsetProvider::ClientInstance_mMinecraftSim);
		if (isValidSim(sim)) return sim;
	} __except (EXCEPTION_EXECUTE_HANDLER) {}

	// 2. Dynamic check for known candidate offsets
	static const int candidateOffsets[] = { 0x1A8, 0x1B0, 0x1A0, 0x1B8, 0x198 };
	for (int off : candidateOffsets) {
		__try {
			auto candidate = hat::member_at<MinecraftSim*>(this, off);
			if (isValidSim(candidate)) return candidate;
		} __except (EXCEPTION_EXECUTE_HANDLER) {}
	}

	// 3. Fallback: check candidate at OffsetProvider only if pointer looks valid
	__try {
		auto sim = hat::member_at<MinecraftSim*>(this, OffsetProvider::ClientInstance_mMinecraftSim);
		if (sim && MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(sim))) return sim;
	} __except (EXCEPTION_EXECUTE_HANDLER) {}

	return nullptr;
}

LevelRenderer* ClientInstance::getLevelRenderer()
{
	if (!this || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(this))) return nullptr;

	if (LevelRendererHook::mInstance) {
		auto lr = LevelRendererHook::mInstance;
		if (MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(lr))) {
			return lr;
		}
		LevelRendererHook::mInstance = nullptr;
	}

	__try {
		if (OffsetProvider::ClientInstance_getLevelRenderer < 297) {
			LevelRenderer* lr = MemUtils::callVirtualFunc<LevelRenderer*>(OffsetProvider::ClientInstance_getLevelRenderer, this);
			if (lr && MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(lr)) && lr != reinterpret_cast<LevelRenderer*>(this)) {
				return lr;
			}
		}
	} __except (EXCEPTION_EXECUTE_HANDLER) {}

	return nullptr;
}

LoopbackPacketSender* ClientInstance::getPacketSender()
{
	if (!this || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(this))) return nullptr;

	static uintptr_t expectedVt = 0;
	if (!expectedVt) {
		__try {
			uintptr_t mc = hat::process::get_process_module().address();
			if (mc) expectedVt = mc + 0x0E890E60;
		} __except (EXCEPTION_EXECUTE_HANDLER) {}
	}

	auto isExactSender = [](LoopbackPacketSender* s) -> bool {
		if (!s || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(s))) return false;
		__try {
			if (expectedVt && *reinterpret_cast<uintptr_t*>(s) == expectedVt) return true;
		} __except (EXCEPTION_EXECUTE_HANDLER) {}
		return false;
	};

	auto isValidSender = [](LoopbackPacketSender* s) -> bool {
		if (!s || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(s))) return false;
		__try {
			if (expectedVt && *reinterpret_cast<uintptr_t*>(s) == expectedVt) return true;
			auto vt = s->vtable;
			if (!vt || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(vt))) return false;
			void* f2 = vt[2];
			if (f2 && MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(f2))) return true;
		} __except (EXCEPTION_EXECUTE_HANDLER) {}
		return false;
	};

	static const int candidateOffsets[] = { 0x1C8, 0x1C0, 0x1D0, 0x1B8, 0x1B0, 0x1D8, 0x1E0, 0x1F0, 0x1A0 };

	// 1. Try exact vtable match on known offsets first
	for (int off : candidateOffsets) {
		__try {
			auto candidate = hat::member_at<LoopbackPacketSender*>(this, off);
			if (isExactSender(candidate)) return candidate;
		} __except (EXCEPTION_EXECUTE_HANDLER) {}
	}

	// 2. Try primary offset from OffsetProvider
	__try {
		auto sender = hat::member_at<LoopbackPacketSender*>(this, OffsetProvider::ClientInstance_mPacketSender);
		if (isValidSender(sender)) return sender;
	} __except (EXCEPTION_EXECUTE_HANDLER) {}

	// 3. Try candidates with generic validation
	for (int off : candidateOffsets) {
		__try {
			auto candidate = hat::member_at<LoopbackPacketSender*>(this, off);
			if (isValidSender(candidate)) return candidate;
		} __except (EXCEPTION_EXECUTE_HANDLER) {}
	}

	// 4. Fallback: virtual function 292 in ClientInstance vtable
	__try {
		auto vtable = *reinterpret_cast<void***>(this);
		if (vtable && MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(vtable))) {
			using Fn = LoopbackPacketSender* (*)(void*);
			auto fn = reinterpret_cast<Fn>(vtable[292]);
			if (fn && MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(fn))) {
				auto vSender = fn(this);
				if (isValidSender(vSender)) return vSender;
			}
		}
	} __except (EXCEPTION_EXECUTE_HANDLER) {}

	return nullptr;
}

GuiData* ClientInstance::getGuiData()
{
	if (!this) return nullptr;
	return hat::member_at<GuiData*>(this, OffsetProvider::ClientInstance_mGuiData);
}

bgfx_context* ClientInstance::getBGFX()
{
	return nullptr;
	//return *reinterpret_cast<bgfx_context**>(SigManager::ClientInstance_mBgfx);
}

ClientInputHandler* ClientInstance::getInputHandler()
{
	if (!this) return nullptr;
	__try {
		if (OffsetProvider::ClientInstance_getInputHandler >= 297) return nullptr;
		return MemUtils::callVirtualFunc<ClientInputHandler*>(OffsetProvider::ClientInstance_getInputHandler, this);
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		return nullptr;
	}
}

KeyboardRemappingLayout* ClientInstance::getKeyboardSettings()
{
	static KeyboardRemappingLayout sDefaultLayout;
	__try {
		auto handler = getInputHandler();
		if (!handler || reinterpret_cast<uintptr_t>(handler) < 0x10000) return &sDefaultLayout;
		auto factory = handler->getMappingFactory();
		if (!factory || reinterpret_cast<uintptr_t>(factory) < 0x10000) return &sDefaultLayout;
		auto layout = factory->getKeyboardRemappingLayout();
		if (!layout || reinterpret_cast<uintptr_t>(layout) < 0x10000) return &sDefaultLayout;
		return layout;
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		return &sDefaultLayout;
	}
}

MinecraftGame* ClientInstance::getMinecraftGame()
{
	return MinecraftGame::getInstance();

}

Actor* ClientInstance::getLocalPlayer()
{
	return MemUtils::callVirtualFunc<Actor*>(OffsetProvider::ClientInstance_getLocalPlayer, this);
}

BlockSource* ClientInstance::getBlockSource()
{
	return MemUtils::callVirtualFunc<BlockSource*>(OffsetProvider::ClientInstance_getBlockSource, this);
}

Options* ClientInstance::getOptions()
{
	if (!this || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(this))) return nullptr;

	auto isValidOptions = [](Options* opt) -> bool {
		if (!opt || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(opt))) return false;
		__try {
			if (!opt->vtable || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(opt->vtable))) return false;
			return true;
		} __except (EXCEPTION_EXECUTE_HANDLER) {
			return false;
		}
	};

	// 1. Try OffsetProvider offset
	__try {
		Options* opt = MemUtils::callVirtualFunc<Options*>(OffsetProvider::ClientInstance_getOptions, this);
		if (isValidOptions(opt)) return opt;
	} __except (EXCEPTION_EXECUTE_HANDLER) {}

	// 2. Try candidate vtable indices (174, 173, 172, 175)
	static const int candidateIndices[] = { 174, 173, 172, 175 };
	for (int idx : candidateIndices) {
		__try {
			Options* opt = MemUtils::callVirtualFunc<Options*>(idx, this);
			if (isValidOptions(opt)) return opt;
		} __except (EXCEPTION_EXECUTE_HANDLER) {}
	}

	return MemUtils::callVirtualFunc<Options*>(OffsetProvider::ClientInstance_getOptions, this);
}

std::string ClientInstance::getPlatformOnlineId()
{
	//return MemUtils::callVirtualFunc<std::string>(144, this);
	using GetPlatformOnlineId_t = void(__fastcall*)(void* clientInstance, std::string* out);
	std::string out;
	auto vf = *reinterpret_cast<uintptr_t**>(this);
	((GetPlatformOnlineId_t)vf[144])(this, &out);
	return out;
}

std::string ClientInstance::getScreenName()
{
	std::string screen = "no_screen";
	MemUtils::callVirtualFunc<std::string*>(OffsetProvider::ClientInstance_getScreenName, this, &screen);
	return screen;
	/* //shit methot
	std::string screen = "no_screen";
	//sig is better
	static auto sig = SigManager::ClientInstance_getScreenName;
	auto fn = reinterpret_cast<std::string & (__thiscall*)(ClientInstance*, std::string&)>(sig);
	screen = fn(this, screen);
	return screen;*/
}


void ClientInstance::setDisableInput(bool disable)
{
	MemUtils::callVirtualFunc<void>(OffsetProvider::ClientInstance_setDisableInput, this, disable);
}

bool ClientInstance::getMouseGrabbed()
{
	auto clickGui = (gFeatureManager && gFeatureManager->mModuleManager) ? gFeatureManager->mModuleManager->getModule<ClickGui>() : nullptr;
	if (clickGui && clickGui->mEnabled) return true;

	auto hudEditor = (gFeatureManager && gFeatureManager->mModuleManager) ? gFeatureManager->mModuleManager->getModule<HudEditor>() : nullptr;
	if (hudEditor && hudEditor->mEnabled) return true;

	std::string screen = getScreenName();
	if (screen != "no_screen" && screen != "hud_screen")
	{
		return true;
	}

	return false;
}

void ClientInstance::grabMouse()
{
	MemUtils::callVirtualFunc<void>(OffsetProvider::ClientInstance_grabMouse, this);
}

void ClientInstance::releaseMouse()
{
	MemUtils::callVirtualFunc<void>(OffsetProvider::ClientInstance_releaseMouse, this);
}

void ClientInstance::playUi(const std::string& soundName, float volume, float pitch)
{   // this is exactly what the game does
	//getMinecraftGame()->playUi(soundName, volume, pitch); // daga kotowaru

	int index = OffsetProvider::ClientInstance_playUi;
	MemUtils::callVirtualFunc<void, const std::string&, float, float>(index, this, soundName, volume, pitch);
}
