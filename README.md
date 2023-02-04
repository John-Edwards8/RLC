# RLC

———

_Проект радиолокационной станции. Исследуется алгоритм поиска целей._

———

## Кратко про текущие файлы и взаимосвязи между модулями:

### Prototypes.h:

— Централизованное подключение библиотек

### Window.h:

— Обработка событий основного окна програмы

### Render.h:

— Отображение рисуемого изображения
— Хранение информации о ячейках сетки (координаты, порядковый номер, наличие или отсутствие цели)

### Objects.h:

— Информация об главных действующих объектах (сетке, луче)
— Формирование изображения сетки и луча
— Движение луча по сетке(?)

### Manager.h:

— Инструмент взаимодействия со всеми классами программы

———

## Кратко про текущие классы и взаимосвязи между ними:

### Класс Window:

#### Поля класса:

int SCREEN_WIDTH и int SCREEN_HEIGHT — размеры окна.

SDL_Window* gWindow — окно SDL.

#### Функции класса:

SDL_Window* getWindow() — геттер объекта окна SDL.

Window \_getWindow() — геттер, возвращающий текущий объект окна.

void setSize(int width, int height) — сеттер размеров окна.

int getWindowWidth() и int getWindowHeight() — геттеры размеров окна.

bool init() — проверка "открылось ли окно SDL".

void handleKeys( auto key ) — обработчик нажатия кнопок.

void close() — уничтожение окна SDL, выход из программы.

void initWindow() — инициализация окна SDL.

### Класс Render:

#### Поля класса:

struct GridList — список ячеек.

struct comp — данные конкретной ячейки.

SDL_Renderer* rend — рендерер SDL.

#### Функции класса:

void \_render() — отображение рисуемого изображения.

void \_clear() — очистка экрана перед отрисовкой.

void constr_list(GridList& l) — конструктор пустого списка ячеек.

inline bool chk_empty(GridList l) — проверка списка ячеек на пустоту.

int struct_len(GridList& l) — количество ячеек.

void comp_in(GridList& l, int X, int Y) — добавление новой "ячейки" в список, задание индексов по ходу наполнения.

### Класс Grid:

#### Поля класса:

int cellWidth, cellHeight — размеры ячеек.

unsigned int cellsInRow, cellsInColumn — количество ячеек в строке и столбце.

int x, y — координаты ячеек.

int bord_x, bord_y — отступ по координатам.

GridList coords — список ячеек.

#### Функции класса:

void startRender()  — "стартовая" отрисовка сетки; создание списка ячеек.

void onlyRender() — отрисовка сетки с использованием имеющегося списка ячеек.

void setBord(int scrW, int scrH) — сеттер отступов по координатам X и Y.

void setCellSize(int scrH) — сеттер размеров ячеек.

int getCellHeight() и int getBord() — геттеры размеров ячеек и отступа (по X).

void setCellsCount(int cellsInRow, int cellsInColumn) — сеттер количества ячеек.

GridList getGridCoords() — геттер списка ячеек.

### Класс Beam:

#### Поля класса:

int x, y — координаты луча.

int radius — радиус луча.

#### Функции класса:

void setValues(int cellH, int bord) — сеттер величин луча (координат начала отрисовки, радиуса).

void startRender() — "стартовая" отрисовка луча.

void render(int newX, int newY) — отрисовка луча по координатам.

void move(Render::GridList l, int curIndex, int cellH) — перемещение луча.

### Класс Manager:

#### Функции класса:

Window getWindow() — геттер, возвращающий текущий объект окна (см. Window::\_getWindow()).

void start() — включение "стартовых" отрисовок.

void setStartValues(int screenWidth, int screenHeight) — задание "стартовых" значений.

void moveBeam() — перемещение и отрисовка луча.

———

### Текущее наследование классов:

Window -> Render -> (Grid,Beam) -> Manager