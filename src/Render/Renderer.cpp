#include "Render/Renderer.h"
#include <string>

namespace Render {
    Display::Display() : Display(800, 600) {}

    Display::Display(unsigned width, unsigned height) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cout << "SDL failed to initialize! SDL error: " << SDL_GetError() << std::endl;
            exit(1);
        }
        _window = SDL_CreateWindow("RLC", width, height, SDL_WINDOW_RESIZABLE);
        if (!_window) {
            std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
            exit(1);
        }
        _renderer = std::make_unique<StandardRenderer>(_window);
        SDL_ShowWindow(_window);
    }

    Display::~Display() {
        if (_window) { SDL_DestroyWindow(_window); _window = nullptr; }
        SDL_Quit();
    }

    StandardRenderer::StandardRenderer(SDL_Window* gWindow) {
        _rend = SDL_CreateRenderer(gWindow, nullptr);
        if (!_rend) {
            std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
            exit(1);
        }
        clear();
    }

    StandardRenderer::~StandardRenderer() {
        if (_rend) { SDL_DestroyRenderer(_rend); _rend = nullptr; }
    }

    // ── вспомогательная: нарисовать filled circle ──────────────────────────
    static void fillCircle(SDL_Renderer* r, int cx, int cy, int radius) {
        for (int dy = -radius; dy <= radius; dy++)
            for (int dx = -radius; dx <= radius; dx++)
                if (dx * dx + dy * dy <= radius * radius)
                    SDL_RenderPoint(r, cx + dx, cy + dy);
    }

    // ── вспомогательная: нарисовать текст цифрами через SDL_RenderPoint ────
    // Рисуем маленькие цифры 3×5 пикселей точками.
    static const uint8_t DIGITS[10][5] = {
        {0b111,0b101,0b101,0b101,0b111}, // 0
        {0b010,0b110,0b010,0b010,0b111}, // 1
        {0b111,0b001,0b111,0b100,0b111}, // 2
        {0b111,0b001,0b111,0b001,0b111}, // 3
        {0b101,0b101,0b111,0b001,0b001}, // 4
        {0b111,0b100,0b111,0b001,0b111}, // 5
        {0b111,0b100,0b111,0b101,0b111}, // 6
        {0b111,0b001,0b001,0b001,0b001}, // 7
        {0b111,0b101,0b111,0b101,0b111}, // 8
        {0b111,0b101,0b111,0b001,0b111}, // 9
    };

    // Рисует число num начиная с (x,y), scale — масштаб пикселя
    static void drawNumber(SDL_Renderer* r, int num, int x, int y, int scale = 1) {
        std::string s = std::to_string(num);
        int cx = x;
        for (char ch : s) {
            int d = ch - '0';
            for (int row = 0; row < 5; row++) {
                for (int col = 0; col < 3; col++) {
                    if (DIGITS[d][row] & (0b100 >> col)) {
                        SDL_FRect px = {
                            (float)(cx + col * scale),
                            (float)(y + row * scale),
                            (float)scale, (float)scale
                        };
                        SDL_RenderFillRect(r, &px);
                    }
                }
            }
            cx += (3 + 1) * scale; // 3px ширина + 1px зазор
        }
    }

    void StandardRenderer::render(const SceneData& scene) {
        if (scene.cells.empty()) {
            std::cerr << "WARNING: Empty scene data!" << std::endl;
            return;
        }

        // ── SETUP: просто показываем сетку + жёлтые цели ──────────────────
        if (scene.state == AppState::SETUP) {
            // Контуры клеток
            SDL_SetRenderDrawColor(_rend, 0x55, 0x55, 0x55, 0xFF);
            for (const auto& cell : scene.cells) {
                SDL_FRect rect = { cell.x, cell.y, cell.size, cell.size };
                SDL_RenderRect(_rend, &rect);
            }
            // Жёлтые цели
            SDL_SetRenderDrawColor(_rend, 0xFF, 0xCC, 0x00, 0xFF);
            for (const auto& cell : scene.cells) {
                if (cell.isTarget) {
                    SDL_FRect rect = { cell.x + 4, cell.y + 4, cell.size - 8, cell.size - 8 };
                    SDL_RenderFillRect(_rend, &rect);
                }
            }
            return;
        }

        // ── REPLAY / FINISHED ─────────────────────────────────────────────

        int cs = (int)scene.cells[0].size; // cell size в пикселях
        int scale = std::max(1, cs / 32);  // масштаб цифр (1 при малых клетках, 2+ при крупных)

        for (const auto& cell : scene.cells) {
            int cx = (int)cell.x;
            int cy = (int)cell.y;

            // 1. Заливка: confidence → оттенок синего/зелёного
            //    0.0 = белый, 1.0 = насыщенный синий
            {
                uint8_t intensity = (uint8_t)(cell.confidence * 200.0);
                SDL_SetRenderDrawColor(_rend, 255 - intensity, 255 - intensity, 255, 0xFF);
                SDL_FRect rect = { cell.x + 1, cell.y + 1, cell.size - 2, cell.size - 2 };
                SDL_RenderFillRect(_rend, &rect);
            }

            // 2. Обнаруженная цель — зелёная заливка поверх
            if (cell.isRevealed) {
                SDL_SetRenderDrawColor(_rend, 0x00, 0xCC, 0x44, 0xFF);
                SDL_FRect rect = { cell.x + 4, cell.y + 4, cell.size - 8, cell.size - 8 };
                SDL_RenderFillRect(_rend, &rect);
            }

            // 3. Ложное обнаружение — оранжевая рамка
            if (cell.isDetected && !cell.isTarget) {
                SDL_SetRenderDrawColor(_rend, 0xFF, 0x88, 0x00, 0xFF);
                SDL_FRect rect = { cell.x + 2, cell.y + 2, cell.size - 4, cell.size - 4 };
                SDL_RenderRect(_rend, &rect);
            }

            // 4. Контур клетки
            SDL_SetRenderDrawColor(_rend, 0x33, 0x33, 0x33, 0xFF);
            SDL_FRect outline = { cell.x, cell.y, cell.size, cell.size };
            SDL_RenderRect(_rend, &outline);

            // 5. Аннотация плана — только если клетка включена в план
            //    Формат: две строки
            //      верхняя (белая) — порядковый номер клетки: #rank
            //      нижняя (жёлтая) — кол-во импульсов: Nимп
            if (cell.planAllocation > 0) {
                int margin = 4;

                // Порядок (#rank) — в левом верхнем углу, белый
                SDL_SetRenderDrawColor(_rend, 0xFF, 0xFF, 0xFF, 0xFF);
                drawNumber(_rend, cell.planVisitRank, cx + margin, cy + margin, scale);

                // Кол-во импульсов — под номером, жёлтый
                SDL_SetRenderDrawColor(_rend, 0xFF, 0xDD, 0x00, 0xFF);
                int labelH = 5 * scale + 2; // высота одной строки цифр
                drawNumber(_rend, cell.planAllocation,
                    cx + margin, cy + margin + labelH + 2, scale);
            }
        }

        // 6. Луч — синий круг
        if (scene.state == AppState::REPLAY || scene.state == AppState::FINISHED) {
            SDL_SetRenderDrawColor(_rend, 0x00, 0x44, 0xFF, 0xFF);
            fillCircle(_rend,
                (int)scene.beam.x, (int)scene.beam.y,
                (int)scene.beam.radius);
        }
    }

    void StandardRenderer::present() { SDL_RenderPresent(_rend); }

    void StandardRenderer::clear() {
        SDL_SetRenderDrawColor(_rend, 0x14, 0x14, 0x19, 0xFF); // тёмный фон
        SDL_RenderClear(_rend);
    }
}
