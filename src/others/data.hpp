#pragma once

#include "scripts/human_controller.hpp"
#include "scripts/controller.hpp"
#include "scripts/backpack_controller.hpp"

class GameData final {
public:
    static GameData* Instance();

    void InitControllers(engine::Entity* player);

    Controller* GetController() { return controller_; }
    HumanController* GetHumanController() { return humanController_.get(); }
    BackpackController* GetBackpackController() { return backpackController_.get(); }

    void ChangeController(Controller* controller) {
        controller_ = controller;
    }

    void SetPlayer(engine::Entity* player) { player_ = player; }
    void ClearPlayer() { player_ = nullptr; }
    engine::Entity* GetPlayer() { return player_; }

    void SetPickableObjGridPos(const engine::Vec2& gridPos) { pickableObjGridPos_ = gridPos; }
    void ClearPickableObjGridPos() { pickableObjGridPos_ = std::nullopt; }
    const std::optional<engine::Vec2>& GetPickableObjGridPos() const { return pickableObjGridPos_; }

    void SetBackpackPanel(engine::Entity* backpack) { backpackPanel_ = backpack; }
    engine::Entity* GetBackpackPanel() { return backpackPanel_; }

private:
    Controller* controller_ = nullptr;
    std::unique_ptr<HumanController> humanController_ = nullptr;
    std::unique_ptr<BackpackController> backpackController_ = nullptr;
    engine::Entity* player_ = nullptr;
    engine::Entity* backpackPanel_ = nullptr;
    std::optional<engine::Vec2> pickableObjGridPos_ = std::nullopt;

    static std::unique_ptr<GameData> instance_;
};

class MonsterManager {
public:
    using iterator = std::vector<engine::Entity*>::iterator;
    using const_iterator = std::vector<engine::Entity*>::const_iterator;

    static iterator begin() { return monsters_.begin(); }
    static iterator end() { return monsters_.end(); }
    static const_iterator cbegin() { return monsters_.cbegin(); }
    static const_iterator cend() { return monsters_.cend(); }

    static void Add(engine::Entity* monster) {
        monsters_.push_back(monster);
        shouldSort_ = true;
    }
    static void Clear() { monsters_.clear(); }
    static void Remove(engine::Entity* monster) {
        auto it = std::find(monsters_.begin(), monsters_.end(), monster);
        if (it != monsters_.end()) {
            monsters_.erase(it);
        }
    }

    static void SortMonsterInYAxis() {
        if (!shouldSort_) return;

        std::sort(monsters_.begin(), monsters_.end(),
        [](const engine::Entity* e1, const engine::Entity* e2){
                auto node1 = e1->GetComponent<engine::Node2DComponent>();
                auto node2 = e2->GetComponent<engine::Node2DComponent>();
                return node1->position.y < node2->position.y;
            });

        shouldSort_ = false;
    }

private:
    static std::vector<engine::Entity*> monsters_;
    static bool shouldSort_;
};