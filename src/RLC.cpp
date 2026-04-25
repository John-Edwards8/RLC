#include "../headers/Manager.h"
#include "Batch/BatchRunner.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

int main( int argc, char* args[] ) {
	if (argc > 1 && std::string(args[1]) == "--batch") {
		BatchConfig cfg;
		cfg.runs = std::stoi(args[2]);
		cfg.rows = std::stoi(args[3]);
		cfg.cols = std::stoi(args[4]);
		cfg.impulses = std::stoi(args[5]);
		cfg.frequency = std::stoi(args[6]);
		cfg.targets = {{1,4}, {3,1}, {4,4}};
		BatchRunner runner(cfg);
		runner.run();
		return 0;
	}
    // --- параметры по умолчанию ---
    int   uiRows = 5;
    int   uiCols = 5;
    int   uiImpulses = 10000;
    int   uiFreq = 100;
    int   uiAlgo = 0; // индекс в комбобоксе

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
        uiImpulses, uiFreq, 1280, 720,
        uiRows, uiCols, algoTypes[uiAlgo]);

    // --- ImGui init ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    SDL_Window* sdlWindow = mainMan->getWindow();
    SDL_Renderer* sdlRenderer = mainMan->getSDLRenderer();

    ImGui_ImplSDL3_InitForSDLRenderer(sdlWindow, sdlRenderer);
    ImGui_ImplSDLRenderer3_Init(sdlRenderer);
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(
        "C:/Windows/Fonts/arial.ttf", 16.0f,
        nullptr,
        io.Fonts->GetGlyphRangesCyrillic()
    );

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL3_ProcessEvent(&e);
            if (e.type == SDL_EVENT_QUIT) { quit = true; break; }

            if (!ImGui::GetIO().WantCaptureMouse) {
                if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                    e.button.button == SDL_BUTTON_LEFT)
                    mainMan->handleClick(
                        static_cast<int>(e.button.x),
                        static_cast<int>(e.button.y));
            }
            if (!ImGui::GetIO().WantCaptureKeyboard) {
                if (e.type == SDL_EVENT_KEY_DOWN) {
                    if (e.key.scancode == SDL_SCANCODE_ESCAPE) quit = true;
                    if (e.key.scancode == SDL_SCANCODE_SPACE && !mainMan->finished())
                        mainMan->startSimulation();
                }
            }
        }

        // --- ImGui frame ---
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({ 10, 10 }, ImGuiCond_Always);
        ImGui::SetNextWindowSize({ 260, 0 }, ImGuiCond_Always);
        ImGui::Begin("Параметри", nullptr,
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        int prevAlgo = uiAlgo;
        int prevRows = uiRows;
        int prevCols = uiCols;
        int prevImp = uiImpulses;
        int prevFreq = uiFreq;

        ImGui::Text("Алгоритм:");
        ImGui::Combo("##algo", &uiAlgo, algoNames, 4);
        ImGui::Separator();
        ImGui::Text("Сітка:");
        ImGui::SliderInt("Рядки##r", &uiRows, 3, 10);
        ImGui::SliderInt("Стовпці##c", &uiCols, 3, 10);
        ImGui::Separator();
        ImGui::Text("Імпульси:");
        ImGui::InputInt("Ліміт##imp", &uiImpulses, 1000);
        ImGui::InputInt("Частота##freq", &uiFreq, 10);
        uiImpulses = std::max(100, uiImpulses);
        uiFreq = std::max(1, uiFreq);

        if (uiAlgo != prevAlgo || uiRows != prevRows || uiCols != prevCols ||
            uiImpulses != prevImp || uiFreq != prevFreq)
        {
            mainMan->reset(uiImpulses, uiFreq, uiRows, uiCols, algoTypes[uiAlgo]);
        }

        ImGui::Separator();
        if (ImGui::Button("Старт", { 120, 30 })) {
            if (!mainMan->finished())
                mainMan->startSimulation();
        }
        ImGui::SameLine();
        if (ImGui::Button("Скинути", { 110, 30 })) {
            mainMan->reset(uiImpulses, uiFreq,
                uiRows, uiCols, algoTypes[uiAlgo]);
        }

        ImGui::Separator();
        ImGui::Text("Стан: %s",
            mainMan->finished() ? "Завершено" :
            mainMan->running() ? "Виконується" : "Налаштування");

        ImGui::End();
        ImGui::Render();

        if (!mainMan->finished()) {
            mainMan->step();
            mainMan->updateScene();
        }
        mainMan->renderScene();

        ImGui_ImplSDLRenderer3_RenderDrawData(
            ImGui::GetDrawData(), sdlRenderer);
        SDL_RenderPresent(sdlRenderer);

        SDL_Delay(16);
    }

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    return 0;
}