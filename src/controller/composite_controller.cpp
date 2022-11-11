#include "composite_controller.hpp"
#include "others/data.hpp"
#include "controller/human_controller.hpp"
#include "others/compose_reader.hpp"

void CompositeController::Update() {
  if (up_) up_->Update();
  if (down_) down_->Update();
  if (left_) left_->Update();
  if (right_) right_->Update();
  if (compose_) compose_->Update();
  if (closePanel_) closePanel_->Update();

  auto compositePanel = GameData::Instance()
                        .GetCompositePanel().Except("composite panel don't exists")
                        ->GetComponent<component::GridPanel>().Except("composite panel don't has GridPanel component");
  changeDescriptionPanelCapacity(compositePanel);
}

void CompositeController::changeDescriptionPanelCapacity(component::GridPanel* panel) {
    // FIXME refactory
    auto& hoverPos = panel->GetHoverGridPos();
    int objectIdx = hoverPos.x + hoverPos.y * panel->gridNumInCol;
    auto it = ComposeConfigStorage::begin();
    std::advance(it, objectIdx);
    auto& materials = it->second.materials;
    auto compositeDescriptionPanel = GameData::Instance().GetCompositeDescriptionPanel().Except("description panel don't exists")
                                                        ->GetComponent<component::GridPanel>().Except("description panel don't has GridPanel component");
    compositeDescriptionPanel->capacity = materials.size();
}