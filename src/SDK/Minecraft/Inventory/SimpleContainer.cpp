//
// Created by vastrakai on 7/6/2024.
//

#include "SimpleContainer.hpp"

#include <SDK/OffsetProvider.hpp>
#include <Utils/MemUtils.hpp>


ItemStack* Container::getItem(int slot)
{
	using func = ItemStack const& (__thiscall*)(void*, int);
	ItemStack const& aids = (*static_cast<func**>((void*)this))[7]((void*)this, slot);
	return const_cast<ItemStack*>(&aids);
}
/*
ItemStack* Container::getItem(int slot)
{
	static auto Func = reinterpret_cast<ItemStack * (__thiscall*)(Container*, int)>((*reinterpret_cast<void***>(this))[7]);
	return Func(this, slot);
}
*/



void Container::setItem(int slot, ItemStack* item)
{
	using SetItemFunc = void(__thiscall*)(Container*, int, ItemStack*);
	auto vFunc = reinterpret_cast<SetItemFunc>((*reinterpret_cast<void***>(this))[12]);
	vFunc(this, slot, item);
}
