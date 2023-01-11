#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <imgui-SFML.h>
#include <imgui.h>
#include <valarray>

// первое множество
static const int SET_1 = 0;
// второе множество
static const int SET_2 = 1;
// пересечение множеств
static const int SET_CROSSED = 2;
// разность множеств
static const int SET_SINGLE = 3;

// Ширина окна
static const int WINDOW_SIZE_X = 800;
// Высота окна
static const int WINDOW_SIZE_Y = 800;

// Точка
struct Point {
    // положение
    sf::Vector2i pos;
    // номер множества
    int setNum;

    // конструктор
    Point(const sf::Vector2i &pos, int setNum) : pos(pos), setNum(setNum) {
    }
};
//окружность
struct Circle {
    // положение
    Point A;
    // положение
    Point B;
    // номер множества
    int setNum;
    double radius;
    double xpos;
    double ypos;
    // конструктор
    Circle(const Point A, Point B, int setNum) : A(A), B(B), setNum(setNum) {
        sf::Vector2i C=A.pos-B.pos;
        sf::Vector2i A1=A.pos;
        radius=sqrt(C.x*C.x+C.y*C.y);
        xpos=A1.x;
        ypos=A1.y;
    };
};

// динамический список точек
std::vector<Point> points;

// динамический список кругов
std::vector <Circle> circles;

// цвет фона
static sf::Color bgColor;
// значение цвета по умолчанию
float color[3] = {0.12f, 0.12f, 0.13f};

// задать цвет фона по вещественному массиву компонент
static void setColor(float *pDouble) {
    bgColor.r = static_cast<sf::Uint8>(pDouble[0] * 255.f);
    bgColor.g = static_cast<sf::Uint8>(pDouble[1] * 255.f);
    bgColor.b = static_cast<sf::Uint8>(pDouble[2] * 255.f);
}

// рисование параметров цвета
void ShowBackgroundSetting() {
    // Инструмент выбора цвета
    if (ImGui::ColorEdit3("Background color", color)) {
        // код вызывается при изменении значения
        // задаём цвет фона
        setColor(color);
    }
    // конец рисование окна
}


// рисование задачи на невидимом окне во всё окно приложения
void RenderTask() {
    // задаём левый верхний край невидимого окна
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    // задаём правый нижний край невидимого окна
    ImGui::SetNextWindowSize(ImVec2(WINDOW_SIZE_X, WINDOW_SIZE_Y));
    // создаём невидимое окно
    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
    // получаем список примитивов, которые будут нарисованы
    auto pDrawList = ImGui::GetWindowDrawList();

    // перебираем точки из динамического массива точек
    for (auto point: points) {
        // добавляем в список рисования круг
        pDrawList->AddCircleFilled(
                sf::Vector2i(point.pos.x, point.pos.y),
                3,
                point.setNum == SET_1 ? ImColor(200, 100, 150) : ImColor(100, 200, 150),
                20
        );
    }
    for (auto circle: circles) {
        // добавляем в список рисования круг
        pDrawList->AddCircle(
                sf::Vector2i(circle.xpos, circle.ypos),
                circle.radius,
                circle.setNum == SET_1 ? ImColor(200, 100, 150) : ImColor(100, 200, 150)
        );
    }


    // заканчиваем рисование окна
    ImGui::End();
}

// главный метод
int main() {
    // создаём окно для рисования
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y), "Geometry Project 10");
    // задаём частоту перерисовки окна
    window.setFramerateLimit(60);
    // инициализация imgui+sfml
    ImGui::SFML::Init(window);

    // задаём цвет фона
    setColor(color);

    points.push_back(Point(sf::Vector2i(180, 600), SET_1));
    points.push_back(Point(sf::Vector2i(100, 700), SET_1));
    points.push_back(Point(sf::Vector2i(200, 500), SET_2));
    points.push_back(Point(sf::Vector2i(200, 700), SET_2));
    circles.push_back(Circle(Point(sf::Vector2i(180, 600), SET_1),Point(sf::Vector2i(200, 500), SET_2), SET_1));
    // опорная точка окружности
    sf::Vector2i pointA = {230, 300};



    // переменная таймера
    sf::Clock deltaClock;
    // пока окно открыто, запускаем бесконечный цикл
    while (window.isOpen()) {
        // создаём событие sfml
        sf::Event event;
        // пока окно принимает события
        while (window.pollEvent(event)) {
            // отправляем события на обработку sfml
            ImGui::SFML::ProcessEvent(event);

            // если событие - это закрытие окна
            if (event.type == sf::Event::Closed) {
                // закрываем окно
                window.close();
            }
        }

        // запускаем обновление окна по таймеру с заданной частотой
        ImGui::SFML::Update(window, deltaClock.restart());

        // рисование задания
        RenderTask();

        // делаем окно полупрозрачным
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.12f, 0.13f, 0.8f));

        // создаём окно управления
        ImGui::Begin("Control");

        // рисование параметров цвета
        ShowBackgroundSetting();

        // конец рисования окна
        ImGui::End();

        // Возвращаем цвет окна к исходному
        ImGui::PopStyleColor();

        // очищаем окно
        window.clear(bgColor);
        // рисуем по окну средствами imgui+sfml
        ImGui::SFML::Render(window);
        // отображаем изменения на окне
        window.display();
    }

    // завершаем работу imgui+sfml
    ImGui::SFML::Shutdown();

    return 0;
}