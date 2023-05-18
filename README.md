# RLC

———

_Проект радиолокационной станции. Исследуется алгоритм поиска целей._

———

## Кратко про текущие файлы и взаимосвязи между модулями:

### Window.h:

— Создание окна и рендерера SDL
— Централизованное подключение библиотек
— Отображение рисуемого изображения

### Objects.h:

— Информация об главных действующих объектах (сетке, луче)
— Хранение информации о ячейках сетки (координаты, вероятность обнаружения цели, наличие или отсутствие цели, информация о том нашли ли цель)
— Формирование изображения сетки и луча
— Вывод сообщений при движении луча по сетке

### Manager.h:

— Инструмент взаимодействия со всеми классами программы
— Логика движения луча по сетке
— Логгирование

———

## Кратко про текущие классы и взаимосвязи между ними:

### Класс Window:

#### Поля класса:

unsigned screenWidth, screenHeight — размеры окна.

SDL_Window* gWindow — окно SDL.

SDL_Renderer* rend — рендерер SDL.

#### Функции класса:

void initWindow() — инициализация окна SDL.

void setValues(unsigned width, unsigned height) — сеттер размеров окна.

bool init() — проверка "открылось ли окно SDL".

void close() — уничтожение окна SDL, выход из программы.

void \_render() — отображение рисуемого изображения.

void reCreate() — повторное создание окна и рендерера.

void \_clear() — очистка экрана перед отрисовкой.

### Структура cell:

#### Поля структуры:

unsigned coordX — координата X.

unsigned coordY — координата Y.

bool target — наличие/отсутствие цели.

bool isFound — найдена/не найдена цель.

double targetChecker — вероятность обнаружения.

### Класс Grid:

#### Поля класса:

unsigned cellWidth, cellHeight — размеры ячеек.

unsigned cellsInRow, cellsInColumn — количество ячеек в строке и столбце.

unsigned x, y — координаты ячеек.

unsigned bordX, bordY — отступ по координатам.

int viewedTargs — количество ячеек, вернувших положительный ответ.

cell** coords — список ячеек.

#### Функции класса:

~Grid() — перегруженный деструктор; удаляет компоненты сетки.

void createCoords() — создание координат для всей сетки.

void onlyRender() — отрисовка сетки с использованием имеющихся ячеек.

void markTargets() — помечает ячейки с целями.

void setValues(unsigned scrW, unsigned scrH, unsigned cellsInRow, unsigned cellsInColumn) — сеттер отступов по координатам X и Y, размеров ячеек, а также их количества.

unsigned getCellHeight(), getBord() — геттеры размеров ячеек и отступа (по X).

unsigned getCellsInRow(), getCellsInColumn() — геттеры количества ячеек.

void recalcTargets(int curIndexColumn, int curIndexRow, int curTargs) — пересчитывает количество целей.

void isTarget(int curIndexColumn, int curIndexRow, int imp, int freq) — проверяет есть ли в клетке цель.

### Класс Beam:

#### Поля класса:

unsigned x, y — координаты луча.

int radius — радиус луча.

#### Функции класса:

void setValues(unsigned cellH, unsigned bord) — сеттер величин луча (координат начала отрисовки, радиуса).

void render(unsigned newX, unsigned newY) — отрисовка луча по координатам.

void move(cell** l, int curIndexColumn, int curIndexRow, unsigned cellH, int impCnt, int dImpCnt, bool first) — перемещение луча.

### Класс Manager:

#### Поля класса:

bool done — найдены ли все цели.

bool logging — начался ли процесс логгирования.

bool first — прошел ли первый период обзора.

int impForCell — кол-во импульсов для клетки без цели.

int doubleImpForCell — кол-во импульсов для клетки с целью.

int targets — кол-во целей которые найдены на текущий момент.

int allImp — все использованные импульсы.

#### Функции класса:

bool init() — проверка инициализации окна SDL.

void close() — закрытие SDL.

void \_render() — рендеринг отрисованого изображения.

void initGrid() — создание сетки.

void renderGrid() — отрисовка сетки.

void mark() — помечает цели.

void log(int cI, int cJ, int imp, int freq, int impCnt, int dImpCnt) — логгирование.

void setValues(int& fr, int& impulse) — задание значений окна, сетки, количества и частоты импульсов.

void moveBeam(int impCnt, int freq) — перемещение и отрисовка луча.

———

### Текущее наследование классов:

Window -> (Grid,Beam) -> Manager