#pragma once

#include "core/pch.hpp"
#include "app/app.hpp"
#include "game/config.hpp"
#include "game/map.hpp"
#include "game/monster.hpp"
#include "game/signal_defs.hpp"
#include "game/input.hpp"

struct LUA_BIND_RESOURCE NearestItemHover {
    bool valid = false;
    math::Vector2 position;
};

void LUA_BIND PlayerMove(const Input& input, Monster& monster, physic::Particle& particle);

void LUA_BIND PickupTileOneItem(Backpack&, ecs::Commands&, ecs::Querier, ecs::Resources, ecs::Events&);

void PutItemIntoBackpack(Backpack&, const Item&, ecs::Commands&, ecs::Querier, ecs::Resources, ecs::Events&);