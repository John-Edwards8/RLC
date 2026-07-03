#include "../headers/Manager.h"
#include "Batch/BatchRunner.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

int main(int argc, char* args[]) {
    if (argc > 1 && std::string(args[1]) == "--batch") {
        BatchConfig cfg;
        cfg.runs = (argc > 2) ? std::stoi(args[2]) : 1000;
        //cfg.duration = (argc > 3) ? std::stod(args[3]) : 1.0;
        //cfg.frequency = (argc > 4) ? std::stoi(args[4]) : 300;
        BatchRunner runner(cfg);
        runner.run();
        return 0;
    }

    int   uiRows = 3;
    int   uiCols = 10;
    float uiDuration = 1.0f;
    int   uiFreq = 300;
    int   uiAlgo = 0;

    const char* algoNames[] = {
        "Послідовний",
        "Макс. елемент",
        "Два функціонали",
        "Вагові коефіцієнти"
    };
    const Core::SolverType algoTypes[] = {
        Core::SolverType::SEQUENTIAL,
        Core::SolverType::MAX_ELEMENT,
        Core::SolverType::TWO_FUNCTIONS,
        Core::SolverType::WEIGHTED_COEFFICIENTS
    };

    auto mainMan = std::make_unique<Manager>(
        uiDuration, uiFreq, 1920, 1080,
        uiRows, uiCols, algoTypes[uiAlgo]);

    // --- ImGui init ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    //ImGui::StyleColorsDark();

    SDL_Window* sdlWindow = mainMan->getWindow();
    SDL_Renderer* sdlRenderer = mainMan->getSDLRenderer();

    ImGui_ImplSDL3_InitForSDLRenderer(sdlWindow, sdlRenderer);
    ImGui_ImplSDLRenderer3_Init(sdlRenderer);
    ImGuiIO& io = ImGui::GetIO();
    static const ImWchar ranges[] = {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2500, 0x25FF, // Geometric Shapes (для ▶ и ⏸)
        0
    };
    io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/arial.ttf", 18.0f, nullptr, ranges);

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL3_ProcessEvent(&e);
            if (e.type == SDL_EVENT_QUIT) { quit = true; break; }

            if (!io.WantCaptureMouse && mainMan->getState() == AppState::SETUP) {
                if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                    e.button.button == SDL_BUTTON_LEFT)
                    mainMan->handleClick(
                        static_cast<int>(e.button.x),
                        static_cast<int>(e.button.y));
            }
            if (!io.WantCaptureKeyboard) {
                if (e.type == SDL_EVENT_KEY_DOWN) {
                    switch (e.key.scancode) {
                    case SDL_SCANCODE_SPACE:
                        if (mainMan->getState() == AppState::SETUP)
                            mainMan->setState(AppState::COMPUTING);
                        else if (mainMan->getState() == AppState::REPLAY)
                            mainMan->togglePause();
                        break;
                    case SDL_SCANCODE_LEFT: mainMan->replayStepBack(); break;
                    case SDL_SCANCODE_RIGHT: mainMan->replayStepForward(); break;
                    case SDL_SCANCODE_UP: mainMan->speedUp(); break;
                    case SDL_SCANCODE_DOWN: mainMan->speedDown(); break;
                    case SDL_SCANCODE_ESCAPE: quit = true; break;
                    default: break;
                    }
                }
            }
        }

        // --- ImGui frame ---
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        float topHeight = 0;
        float bottomHeight = 100.0f;
        float rightWidth = 480.0f;

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Файл")) {
                if (ImGui::MenuItem("Зберегти звіт")) { /* ... */ }
                if (ImGui::MenuItem("Старі звіти")) { /* ... */ }
                ImGui::EndMenu();
            }
            ImGui::TextDisabled("|");
            ImGui::Text("Стан: %s", mainMan->getState() == AppState::SETUP ? "Налаштування" :
                mainMan->getState() == AppState::COMPUTING ? "Обчислення" : "Аналіз");
            topHeight = ImGui::GetWindowSize().y;
            ImGui::EndMainMenuBar();
        }

        // --- 2. ПРАВАЯ ПАНЕЛЬ (Setup) ---
        ImGui::SetNextWindowPos({ viewport->WorkPos.x + viewport->WorkSize.x - rightWidth, viewport->WorkPos.y });
        ImGui::SetNextWindowSize({ rightWidth, viewport->WorkSize.y - bottomHeight });
        ImGui::Begin("Налаштування сцени", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

        if (mainMan->getState() == AppState::SETUP) {
            int pAlgo = uiAlgo, pR = uiRows, pC = uiCols, pD = uiDuration, pF = uiFreq;

            ImGui::Combo("Алгоритм", &uiAlgo, algoNames, 4);
            ImGui::SliderInt("Рядки", &uiRows, 3, 15);
            ImGui::SliderInt("Стовпці", &uiCols, 3, 15);

            ImGui::Separator();

            ImGui::SliderFloat("Тривалість (сек)", &uiDuration, 0.1f, 10.0f);
            ImGui::NewLine();
            ImGui::InputInt("Частота (Гц)", &uiFreq, 10);

            if (uiAlgo != pAlgo || uiRows != pR || uiCols != pC ||
                uiDuration != pD || uiFreq != pF)
            {
                mainMan->reset(uiDuration, uiFreq, uiRows, uiCols, algoTypes[uiAlgo]);
            }

            ImGui::Separator();
            bool showPlan = mainMan->isShowPlan();
            if (ImGui::Checkbox("Показати план [P]", &showPlan))
                mainMan->toggleShowPlan();

            ImGui::Dummy({ 0, 10 });
            if (ImGui::Button("РОЗПОЧАТИ ОБЧИСЛЕННЯ", { -1, 50 }))
                mainMan->setState(AppState::COMPUTING);

            // Пояснение плана
            if (mainMan->isShowPlan()) {
                int replaySweep = mainMan->replayCurrentSweep();
                const auto& plan = mainMan->getPlanForSweep(replaySweep);
                if (plan.sweepNumber > 0) {
                    ImGui::Separator();
                    ImGui::Text("План прогону %d:", plan.sweepNumber);
                    ImGui::TextWrapped("Число у клітинці = кількість призначених імпульсів.");
                }
            }
        }
        else {
            ImGui::TextWrapped("Налаштування заблоковані. Скиньте для змін.");
            if (ImGui::Button("Скинути все", { -1, 30 }))
                mainMan->reset(uiDuration, uiFreq, uiRows, uiCols, algoTypes[uiAlgo]);

            ImGui::Separator();
            ImGui::Text("Тривалість: %.2f сек", uiDuration);

            bool showPlan = mainMan->isShowPlan();
            if (ImGui::Checkbox("Показати план [P]", &showPlan))
                mainMan->toggleShowPlan();

            int replaySweep = mainMan->replayCurrentSweep();
            const auto& plan = mainMan->getPlanForSweep(replaySweep);
            if (plan.sweepNumber > 0) {
                ImGui::Separator();
                ImGui::Text("Поточний план: прогін %d", plan.sweepNumber);
                // Таблиця розподілу
                if (ImGui::BeginTable("PlanTable", plan.allocations.empty() ? 1
                    : (int)plan.allocations[0].size(),
                    ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit
                    | ImGuiTableFlags_ScrollX))
                {
                    for (int c = 0; c < (int)plan.allocations[0].size(); c++)
                        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 38.0f);
                    for (int r = 0; r < (int)plan.allocations.size(); r++) {
                        ImGui::TableNextRow();
                        for (int c = 0; c < (int)plan.allocations[r].size(); c++) {
                            ImGui::TableNextColumn();
                            int alloc = plan.allocations[r][c];
                            int order = plan.visitRank[r][c];
                            if (alloc > 0)
                                ImGui::TextColored({ 0.3f,0.9f,0.3f,1.0f }, "%d:%d", order, alloc);
                            else
                                ImGui::TextDisabled("-");
                        }
                    }
                    ImGui::EndTable();
                }
            }
        }
        ImGui::End();

        ImGui::SetNextWindowPos({ viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y - bottomHeight });
        ImGui::SetNextWindowSize({ viewport->WorkSize.x, bottomHeight });
        ImGui::Begin("Управління відтворенням", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

        if (mainMan->getState() == AppState::REPLAY || mainMan->getState() == AppState::FINISHED) {
            // Таймлайн
            float progress = mainMan->historySize() > 0 ? (float)mainMan->replayFrame() / mainMan->historySize() : 0.0f;
            ImGui::PushItemWidth(-1);
            if (ImGui::SliderFloat("##timeline", &progress, 0.0f, 1.0f, "")) {
                mainMan->setReplayFrame((int)(progress * mainMan->historySize()));
            }
            ImGui::PopItemWidth();

            // Кнопки управления
            ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - 100);
            if (ImGui::Button(" |<< ")) mainMan->resetReplay(); ImGui::SameLine();
            if (ImGui::Button(" << "))  mainMan->replayStepBack(); ImGui::SameLine();

            // Кнопка Play/Pause с корректным символом
            const char* lbl = mainMan->replayPaused() ? "  ▶  " : "  ⏸  ";
            if (ImGui::Button(lbl, { 40, 0 })) mainMan->togglePause(); ImGui::SameLine();
            if (ImGui::Button(" >> "))  mainMan->replayStepForward(); ImGui::SameLine();
            if (ImGui::Button(" >>| ")) mainMan->resetEndReplay();

            // Скорость
            ImGui::SameLine(ImGui::GetWindowSize().x - 180);
            ImGui::Text("x%.2f", mainMan->replaySpeed());
            ImGui::SameLine();
            if (ImGui::Button(" - ")) mainMan->speedDown(); ImGui::SameLine();
            if (ImGui::Button(" + ")) mainMan->speedUp();
        }
        else {
            ImGui::TextColored({ 0.5f,0.5f,0.5f,1.0f }, "Чекаємо на запуск...");
        }
        ImGui::End();

        // --- COMPUTING PHASE ---
        if (mainMan->getState() == AppState::COMPUTING) {
            ImGui::OpenPopup("ComputingPopup");
            if (ImGui::BeginPopupModal("ComputingPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Тривають обчислення алгоритму...");
                ImGui::ProgressBar(-1.0f * (float)SDL_GetTicks() / 1000.0f, { 300, 20 }, "Рахуємо историю...");
                if (!mainMan->isComputeDone()) {
                    mainMan->startSimulation();
                }
                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
        }

        // --- RENDER SCENE ---
        SDL_SetRenderDrawColor(sdlRenderer, 20, 20, 25, 255);
        SDL_RenderClear(sdlRenderer);

        if (mainMan->getState() != AppState::COMPUTING) {
            if (mainMan->getState() == AppState::REPLAY) {
                mainMan->replayTick();
            }
            mainMan->renderScene();
        }

        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), sdlRenderer);
        SDL_RenderPresent(sdlRenderer);
        SDL_Delay(10);
    }

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    return 0;
}