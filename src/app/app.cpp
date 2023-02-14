#include "app/app.hpp"

void ExitTrigger::DetectExitSystem(ecs::Commands& cmd, ecs::Queryer queryer,
                                   ecs::Resources resources,
                                   ecs::Events& events) {
    if (!resources.Has<ExitTrigger>()) {
        return;
    }
    auto reader = events.Reader<SDL_QuitEvent>();
    auto& trigger = resources.Get<ExitTrigger>();
    if (reader.Has()) {
        trigger.Exit();
    }
}

void EventUpdateSystem(ecs::Commands& cmd, ecs::Queryer queryer,
                       ecs::Resources resources, ecs::Events& events) {
    SDL_Event event;

    KeyboardEvents keyboard;
    MouseBtnEvents mouse;

    ExtraEventHandler* handler = resources.Has<ExtraEventHandler>() ? &resources.Get<ExtraEventHandler>() : nullptr;

    if (handler) {
        handler->Prepare(resources);
    }

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            events.Writer<SDL_QuitEvent>().Write(event.quit);
        }
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            keyboard.events.push_back(event.key);
        }
        if (event.type == SDL_MOUSEMOTION) {
            events.Writer<SDL_MouseMotionEvent>().Write(event.motion);
        }
        if (event.type == SDL_MOUSEBUTTONDOWN ||
            event.type == SDL_MOUSEBUTTONUP) {
            mouse.events.push_back(event.button);
        }

        if (handler) {
            handler->HandleEvent(event);
        }
    }

    if (handler) {
        handler->Finish(resources);
    }

    if (keyboard) {
        events.Writer<KeyboardEvents>().Write(keyboard);
    }
    if (mouse) {
        events.Writer<MouseBtnEvents>().Write(mouse);
    }
}

void DefaultPlugins::Build(ecs::World* world) {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    world->SetResource(Window("Grogue", 1024, 720))
        .SetResource(AssetsManager{})
        .SetResource(BGMPlayer{world->GetResource<AssetsManager>()->BGM()})
        .SetResource(Renderer{*world->GetResource<Window>(),
                              world->GetResource<AssetsManager>()->Font()})
        .SetResource(Keyboard{})
        .SetResource(Mouse{})
        .SetResource(ExitTrigger{})
        .SetResource(Timer{});

    auto* assets = world->GetResource<AssetsManager>();
    assets->image_ = std::unique_ptr<ImageManager>(
        new ImageManager(*world->GetResource<Renderer>()));

    world->SetResource(TileSheetManager{assets->Image(), assets->Lua()})
        .AddSystem(EventUpdateSystem)
        .AddSystem(Keyboard::UpdateSystem)
        .AddSystem(Mouse::UpdateSystem)
        .AddSystem(Timer::UpdateSystem);
    world->GetResource<Renderer>()->imageManager_ = &assets->Image();
}

void DefaultPlugins::Quit(ecs::World* world) {
    IMG_Quit();
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}

void App::Run() {
    world_.Startup();
    world_.Update();

    ecs::Resources resources(world_);
    if (!resources.Has<ExitTrigger>()) {
        LOGF("You don't have ExitTrigger resource! Please add DefaultPlugin to "
             "your ECS");
        return;
    }

    auto& exit = resources.Get<ExitTrigger>();
    while (!exit.ShouldExit()) {
        auto renderer = world_.GetResource<Renderer>();

        renderer->SetDrawColor(Color(50, 50, 50));
        renderer->Clear();

        world_.Update();

        renderer->Present();
    }
}