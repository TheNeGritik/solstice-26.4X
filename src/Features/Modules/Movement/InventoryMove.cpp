//
// Created by vastrakai on 7/12/2024.
//

#include "InventoryMove.hpp"

#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Events/RenderEvent.hpp>
#include <Features/FeatureManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/KeyboardRemappingLayout.hpp>
#include <SDK/Minecraft/Network/Packets/ContainerClosePacket.hpp>
#include <SDK/SigManager.hpp>


void InventoryMove::Patch() {

	if (Patched) {
		return;
	}
	Patched = true;

	std::vector<unsigned char> Bytes = { 0x90, 0x90 };
	std::vector<unsigned char> Bytes2 = { 0x81 }; //jmp 0x146698933 -> 0x1466988EB
	std::vector<unsigned char> Bytes3 = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

	if (SigManager::PlayerMovement_clearInputStateInlined) {
		OriginalBytes = MemUtils::readBytes(SigManager::PlayerMovement_clearInputStateInlined, sizeof(Bytes));
		MemUtils::writeBytes(SigManager::PlayerMovement_clearInputStateInlined, Bytes);
	}
	if (SigManager::PlayerMovement_clearInputStateInlined2) {
		OriginalBytes2 = MemUtils::readBytes(SigManager::PlayerMovement_clearInputStateInlined2 + 1, sizeof(Bytes2));
		MemUtils::writeBytes(SigManager::PlayerMovement_clearInputStateInlined2 + 1, Bytes2);
	}
	if (SigManager::PlayerMovement_clearInputStateInlined3) {
		OriginalBytes3 = MemUtils::readBytes(SigManager::PlayerMovement_clearInputStateInlined3, sizeof(Bytes3));
		MemUtils::writeBytes(SigManager::PlayerMovement_clearInputStateInlined3, Bytes3);
	}
}

void InventoryMove::UnPatch() {

	if (!Patched) {
		return;
	}
	Patched = false;

	if (SigManager::PlayerMovement_clearInputStateInlined && !OriginalBytes.empty()) {
		MemUtils::writeBytes(SigManager::PlayerMovement_clearInputStateInlined, OriginalBytes);
	}
	if (SigManager::PlayerMovement_clearInputStateInlined2 && !OriginalBytes2.empty()) {
		MemUtils::writeBytes(SigManager::PlayerMovement_clearInputStateInlined2 + 1, OriginalBytes2);
	}
	if (SigManager::PlayerMovement_clearInputStateInlined3 && !OriginalBytes3.empty()) {
		MemUtils::writeBytes(SigManager::PlayerMovement_clearInputStateInlined3, OriginalBytes3);
	}
}

void InventoryMove::onEnable()
{
	Patch();
	gFeatureManager->mDispatcher->listen<BaseTickEvent, &InventoryMove::onBaseTickEvent>(this);
	gFeatureManager->mDispatcher->listen<RenderEvent, &InventoryMove::onRenderEvent>(this);
	gFeatureManager->mDispatcher->listen<PacketInEvent, &InventoryMove::onPacketInEvent>(this);
	gFeatureManager->mDispatcher->listen<PacketOutEvent, &InventoryMove::onPacketOutEvent>(this);
}

void InventoryMove::onDisable()
{
	UnPatch();

	gFeatureManager->mDispatcher->deafen<BaseTickEvent, &InventoryMove::onBaseTickEvent>(this);
	gFeatureManager->mDispatcher->deafen<RenderEvent, &InventoryMove::onRenderEvent>(this);
	gFeatureManager->mDispatcher->deafen<PacketInEvent, &InventoryMove::onPacketInEvent>(this);
	gFeatureManager->mDispatcher->deafen<PacketOutEvent, &InventoryMove::onPacketOutEvent>(this);
}

void InventoryMove::onBaseTickEvent(BaseTickEvent& event)
{
	if (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantTextInput) return;
	auto player = event.mActor;
	if (!player) return;

	auto input = player->getMoveInputComponent();
	if (!input) return;

	auto kSettings = ClientInstance::get()->getKeyboardSettings();
	auto getKey = [&](const std::string& name, int def) -> int {
		if (kSettings) {
			int k = (*kSettings)[name];
			if (k > 0 && k < 256) return k;
		}
		return def;
	};

	int keyW = getKey("key.forward", 'W');
	int keyA = getKey("key.left", 'A');
	int keyS = getKey("key.back", 'S');
	int keyD = getKey("key.right", 'D');
	int keySpace = getKey("key.jump", VK_SPACE);
	int keyShift = getKey("key.sneak", VK_SHIFT);

	bool w = (keyW > 0 && keyW < 256) && Keyboard::mPressedKeys[keyW];
	bool a = (keyA > 0 && keyA < 256) && Keyboard::mPressedKeys[keyA];
	bool s = (keyS > 0 && keyS < 256) && Keyboard::mPressedKeys[keyS];
	bool d = (keyD > 0 && keyD < 256) && Keyboard::mPressedKeys[keyD];
	bool space = (keySpace > 0 && keySpace < 256) && Keyboard::mPressedKeys[keySpace];
	bool shift = (keyShift > 0 && keyShift < 256) && Keyboard::mPressedKeys[keyShift];
	bool pressed = w || a || s || d || space || shift;

	std::string screenName = ClientInstance::get()->getScreenName();
	bool isInChatScreen = screenName == "chat_screen";
	if (isInChatScreen) return;

	if (isInChatScreen || !pressed)
	{
		input->setUp(false);
		input->setDown(false);
		input->setLeft(false);
		input->setRight(false);
		input->setJumping(false);
		input->setSneaking(false);
		input->mMove = glm::vec2();
		input->mInputState.AnalogMoveVector = glm::vec2();
		input->mRawInputState.AnalogMoveVector = glm::vec2();
		return;
	}

	input->setUp(w);
	input->setDown(s);
	input->setLeft(a);
	input->setRight(d);
	input->setJumping(space);
	input->setSneaking(shift && !mDisallowShift ? true : false);
	input->mMove = MathUtils::getMovement();
	input->mInputState.AnalogMoveVector = MathUtils::getMovement();
	input->mRawInputState.AnalogMoveVector = MathUtils::getMovement();
}

void InventoryMove::onRenderEvent(RenderEvent& event)
{
	if (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantTextInput) return;
	auto player = ClientInstance::get()->getLocalPlayer();
	if (!player) return;

	auto input = player->getMoveInputComponent();
	if (!input) return;

	auto kSettings = ClientInstance::get()->getKeyboardSettings();
	auto getKey = [&](const std::string& name, int def) -> int {
		if (kSettings) {
			int k = (*kSettings)[name];
			if (k > 0 && k < 256) return k;
		}
		return def;
	};

	int keyW = getKey("key.forward", 'W');
	int keyA = getKey("key.left", 'A');
	int keyS = getKey("key.back", 'S');
	int keyD = getKey("key.right", 'D');
	int keySpace = getKey("key.jump", VK_SPACE);
	int keyShift = getKey("key.sneak", VK_SHIFT);

	bool w = (keyW > 0 && keyW < 256) && Keyboard::mPressedKeys[keyW];
	bool a = (keyA > 0 && keyA < 256) && Keyboard::mPressedKeys[keyA];
	bool s = (keyS > 0 && keyS < 256) && Keyboard::mPressedKeys[keyS];
	bool d = (keyD > 0 && keyD < 256) && Keyboard::mPressedKeys[keyD];
	bool space = (keySpace > 0 && keySpace < 256) && Keyboard::mPressedKeys[keySpace];
	bool shift = (keyShift > 0 && keyShift < 256) && Keyboard::mPressedKeys[keyShift];
	bool pressed = w || a || s || d || space || shift;

	std::string screenName = ClientInstance::get()->getScreenName();
	bool isInChatScreen = screenName == "chat_screen";
	if (isInChatScreen) return;

	if (isInChatScreen || !pressed)
	{
		input->setUp(false);
		input->setDown(false);
		input->setLeft(false);
		input->setRight(false);
		input->setJumping(false);
		input->setSneaking(false);
		input->mMove = glm::vec2();
		input->mInputState.AnalogMoveVector = glm::vec2();
		input->mRawInputState.AnalogMoveVector = glm::vec2();
		return;
	}

	input->setUp(w);
	input->setDown(s);
	input->setLeft(a);
	input->setRight(d);
	input->setJumping(space);
	input->setSneaking(shift && !mDisallowShift ? true : false);
	input->mMove = MathUtils::getMovement();
	input->mInputState.AnalogMoveVector = MathUtils::getMovement();
	input->mRawInputState.AnalogMoveVector = MathUtils::getMovement();
}

void InventoryMove::onPacketInEvent(PacketInEvent& event)
{
	if (event.mPacket->getId() == PacketID::ContainerOpen)
	{
		auto packet = event.getPacket<ContainerOpenPacket>();
		mHasOpenContainer = true;
	}
	if (event.mPacket->getId() == PacketID::ContainerClose)
	{
		mHasOpenContainer = false;
	}
}

void InventoryMove::onPacketOutEvent(PacketOutEvent& event)
{
	if (event.mPacket->getId() == PacketID::ContainerClose)
	{
		mHasOpenContainer = false;
	}
	else if (event.mPacket->getId() == PacketID::ContainerOpen)
	{
		auto packet = event.getPacket<ContainerOpenPacket>();
		mHasOpenContainer = true;
	}
}
