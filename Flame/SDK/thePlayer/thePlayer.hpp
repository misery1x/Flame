#pragma once

#include <string>
#include <vector>
#include "../Entity.hpp"
#include "../ItemStack/ItemStack.hpp"

class CEntityPlayer : public CEntity
{
public:
    CEntityPlayer() : CEntity() {}
    CEntityPlayer(JNIEnv* env, jobject obj) : CEntity(env, obj) {}

    CItemStack getHeldItem();
    int  getHurtTime();
    bool isOnGround();
};
