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


//прямая
struct Line{
    // положение
    Point A;
    // положение
    Point B;
    int setNum;
    sf::Vector2i A1=A.pos;
    sf::Vector2i B1=B.pos;
    double maxDistance = std::sqrt(WINDOW_SIZE_X * WINDOW_SIZE_X + WINDOW_SIZE_Y * WINDOW_SIZE_Y);
    sf::Vector2i renderPointA = sf::Vector2i(
            A1.x + (int) ((A1.x - B1.x) * maxDistance),
            A1.y + (int) ((A1.y - B1.y) * maxDistance)
    );
    sf::Vector2i renderPointB = sf::Vector2i(
            A1.x - (int) ((A1.x - B1.x) * maxDistance),
            A1.y - (int) ((A1.y - B1.y) * maxDistance)
    );
    Line(const Point A, Point B, int setNum) : A(A),B(B), setNum(setNum) {
        double renderAx=renderPointA.x;
        double renderAy=renderPointA.y;
        double renderBx=renderPointB.x;
        double renderBy=renderPointB.y;
    };
};

//отрезок
struct LineSegment{
    // положение
    Point A;
    // положение
    Point B;
    sf::Vector2i A1=A.pos;
    sf::Vector2i B1=B.pos;
    LineSegment(const Point A, Point B) : A(A), B(B) {
        double Ax=A1.x;
        double Ay=A1.y;
        double Bx=B1.x;
        double By=B1.y;
    };
};


// буфер, хранящий координаты последней добавленной вершины
int lastAddPosBuf[2] = {0, 0};

// динамический список точек
std::vector<Point> points;

// динамический список кругов
std::vector <Circle> circles;

// динамический список прямых
std::vector <Line> lines;

// динамический список отрезков
std::vector <LineSegment> linesegments;

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
    // если не раскрыт список `Background`
    if (!ImGui::CollapsingHeader("Background"))
        // заканчиваем выполнение
        return;

    // Инструмент выбора цвета
    if (ImGui::ColorEdit3("Background color", color)) {
        // код вызывается при изменении значения
        // задаём цвет фона
        setColor(color);
    }
    // конец рисование окна
}

// ручное добавление элементов
void ShowAddElem() {

    // если не раскрыта панель `Add Elem`
    if (!ImGui::CollapsingHeader("Add Elem"))
        // заканчиваем выполнение
        return;


    // Инструмент выбора цвета
    if (ImGui::DragInt2("Coords", lastAddPosBuf)) {
        // никаких действий не требуется, достаточно
        // тех изменений буфера, которые imGui выполняет
        // автоматически
    }

    // если нажата кнопка `Set 1`
    if (ImGui::Button("Point"))
        // добавляем то добавляем в список точку, принадлежащую первому множеству
        points.emplace_back(Point(sf::Vector2i(lastAddPosBuf[0], lastAddPosBuf[1]), SET_1));

    // если нажата кнопка `Set 2`
    if (ImGui::Button("Circle"))
        // добавляем то добавляем в список точку, принадлежащую второму множеству
        points.emplace_back(Point(sf::Vector2i(lastAddPosBuf[0], lastAddPosBuf[1]), SET_2));

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

    // перебираем окружности из динамического массива точек
    for (auto circle: circles) {
        // добавляем в список рисования окружность
        pDrawList->AddCircle(
                sf::Vector2i(circle.xpos, circle.ypos),
                circle.radius,
                circle.setNum == SET_1 ? ImColor(200, 100, 150) : ImColor(100, 200, 150)
        );
    }

    // перебираем прямые из динамического массива точек
    for (auto line: lines) {
        // добавляем в список рисования отезок
        pDrawList->AddLine(
                sf::Vector2i(line.renderPointA.x, line.renderPointA.y),
                sf::Vector2i(line.renderPointB.x, line.renderPointB.y),
                line.setNum == SET_1 ? ImColor(200, 100, 150) : ImColor(100, 200, 150),
                0.5f
        );
    }

    // перебираем прямые из динамического массива отрезков
    for (auto linesegment: linesegments) {
        // добавляем в список рисования отезок
        pDrawList->AddLine(
                sf::Vector2i(linesegment.A1.x, linesegment.A1.y),
                sf::Vector2i(linesegment.B1.x, linesegment.B1.y),
                ImColor(19, 19, 228) ,
                0.5f
        );
    }


    // заканчиваем рисование окна
    ImGui::End();
}

// главный метод
int main() {

    //переменные для обработки мышью
    int countpoint=0;
    double centerx=0;
    double centery=0;

    // создаём окно для рисования
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y), "Geometry Project 10");
    // задаём частоту перерисовки окна
    window.setFramerateLimit(60);
    // инициализация imgui+sfml
    ImGui::SFML::Init(window);

    // задаём цвет фона
    setColor(color);

    //пробные объекты
    //points.push_back(Point(sf::Vector2i(180, 600), SET_1));
    //points.push_back(Point(sf::Vector2i(100, 700), SET_1));
    //points.push_back(Point(sf::Vector2i(200, 500), SET_2));
    //points.push_back(Point(sf::Vector2i(200, 700), SET_2));
    //circles.push_back(Circle(Point(sf::Vector2i(190, 550), SET_1),Point(sf::Vector2i(230, 558), SET_2), SET_1));
    //circles.push_back(Circle(Point(sf::Vector2i(460, 350), SET_1),Point(sf::Vector2i(290, 518), SET_2), SET_2));
    //lines.push_back(Line(Point(sf::Vector2i(460, 350), SET_1),Point(sf::Vector2i(290, 518), SET_2), SET_2));
    //lines.push_back(Line(Point(sf::Vector2i(234, 350), SET_1),Point(sf::Vector2i(310, 591), SET_2), SET_1));
    //linesegments.push_back(LineSegment(Point(sf::Vector2i(204, 450), SET_1),Point(sf::Vector2i(300, 591), SET_1)));

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
            // если событие - это клик мышью
            if (event.type == sf::Event::MouseButtonPressed) {
                // если мышь не обрабатывается элементами imGui
                if (!ImGui::GetIO().WantCaptureMouse) {
                    // меняем координаты последней добавленной точки
                    lastAddPosBuf[0] = event.mouseButton.x;
                    lastAddPosBuf[1] = event.mouseButton.y;
                    // если левая кнопка мыши
                    if (event.mouseButton.button == sf::Mouse::Button::Left)
                        points.emplace_back(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), SET_1);
                    else
                    if(countpoint==1){
                        circles.emplace_back(Point(sf::Vector2i(centerx, centery), SET_1), Point(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), SET_1), SET_2);
                        countpoint=0;
                    }else{
                        countpoint=1;
                        centerx=event.mouseButton.x;
                        centery=event.mouseButton.y;
                    }
                }
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
        // ручное добавление элементов
        ShowAddElem();
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