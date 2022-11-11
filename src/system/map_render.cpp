#include "map_render.hpp"

void MapRenderSystem::Update(engine::Entity* entity) {
    if (!entity) return;

    auto mapResult = entity->GetComponent<component::MapComponent>();

    if (mapResult.IsErr()) return;

    auto& map = mapResult.Unwrap();

    auto visiableArea = CalcVisiableTileArea(map->map, GameData::Instance().GetCamera().GetPosition());
    drawTileAndPickupableObj(visiableArea, map->map);
    drawMonsterAndArch(visiableArea, map->map);
}

void MapRenderSystem::drawOneTile(const engine::Image& image,  const engine::Transform& transform) {
    engine::Renderer::DrawTexture(*image.texture, &image.region, engine::Size(TileSize, TileSize), transform);
}

void MapRenderSystem::drawOneSprite(const component::Sprite* sprite, const engine::Transform& transform) {
    engine::Renderer::DrawTexture(*sprite->image.texture, &sprite->image.region, sprite->size, transform);
}

void MapRenderSystem::drawTileAndPickupableObj(const VisiableTileArea& visiableArea, Map* map) {
    for (int y = visiableArea.initRows; y < visiableArea.endRows; y++) {
        for (int x = visiableArea.initCols; x < visiableArea.endCols; x++) {
            if (!map->IsInMap(x, y)) continue;
            
            auto& tile = map->Get(x, y);
            auto transform = engine::Transform{engine::Vec2(x - visiableArea.initCols,
                                                            y - visiableArea.initRows) *
                                               TileSize + visiableArea.offset};
            if (tile.terrian.image) {
                drawOneTile(tile.terrian.image, transform);
            }
            if (tile.object && tile.object->GetComponent<component::Pickupable>().IsOk()) {
                auto sprite = tile.object->GetComponent<component::Sprite>();
                if (sprite.IsOk()) {
                    drawOneSprite(sprite.Unwrap(), transform);
                }
            }
        }
    }
}

void MapRenderSystem::drawMonsterAndArch(const VisiableTileArea& visiableArea, Map* map) {
    auto monsterIt = MonsterManager::begin();

    engine::Node2DComponent* monsterNode = nullptr;
    component::Sprite* monsterSprite = nullptr;

    getNextMonsterInfo(monsterIt, monsterNode, monsterSprite);

    for (int y = visiableArea.initRows; y < visiableArea.endRows; y++) {
        std::optional<float> monsterBottom;
        if (monsterNode && monsterSprite) {
            monsterBottom = monsterNode->globalPosition.y + monsterSprite->size.h;
        }
        while (monsterIt != MonsterManager::end() && monsterBottom &&
               monsterBottom.value() >= y * TileSize && monsterBottom.value() <= (y + 1) * TileSize) {
            drawOneSprite(monsterSprite, {monsterNode->globalPosition - engine::Size(TileSize * visiableArea.initCols, TileSize * visiableArea.initRows) + visiableArea.offset,
                                          monsterNode->globalScale,
                                          monsterNode->globalRotation});

            monsterIt ++;
            getNextMonsterInfo(monsterIt, monsterNode, monsterSprite);
        }
        for (int x = visiableArea.initCols; x < visiableArea.endCols; x++) {
            auto& tile = map->Get(x, y);
            if (tile.object && tile.object->GetComponent<component::Architecture>().IsOk()) {
                auto sprite = tile.object->GetComponent<component::Sprite>();
                if (sprite.IsOk()) {
                    drawOneSprite(sprite.Unwrap(), {engine::Vec2(x - visiableArea.initCols, y - visiableArea.initRows) * TileSize + visiableArea.offset});
                }
            }
        }
    }
}

void MapRenderSystem::getNextMonsterInfo(MonsterManager::iterator& it, Node2DPtr& node, SpritePtr& sprite) {
    if (it != MonsterManager::end()) {
        node = (*it)->GetComponent<engine::Node2DComponent>().Unwrap();
        sprite = (*it)->GetComponent<component::Sprite>().Unwrap();
    }
}
