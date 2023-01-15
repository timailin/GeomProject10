#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <imgui-SFML.h>
#include <imgui.h>
#include <valarray>
#include <iostream>
#include <fstream>
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

    // получить случайную точку
    static Point randomPoint() {
        return Point(sf::Vector2i(
                             rand() % WINDOW_SIZE_X,
                             rand() % WINDOW_SIZE_Y),
                     SET_1
        );

    };
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
    double rxpos;
    double rypos;
    // конструктор
    Circle(const Point A, Point B, int setNum) : A(A), B(B), setNum(setNum) {
        sf::Vector2i C=A.pos-B.pos;
        sf::Vector2i A1=A.pos;
        sf::Vector2i A2=B.pos;
        radius=sqrt(C.x*C.x+C.y*C.y);
        xpos=A1.x;
        ypos=A1.y;
        rxpos=A2.x;
        rypos=A2.y;
    };
    // получить случайную точку
    static Circle randomCircle() {
        return Circle(Point(sf::Vector2i(
                              rand() % WINDOW_SIZE_X,
                              rand() % WINDOW_SIZE_Y),SET_1),
                      Point(sf::Vector2i(
                              rand() % WINDOW_SIZE_X,
                              rand() % WINDOW_SIZE_Y),SET_1),
                      SET_2
        );
    }
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

// путь к файлу вывода точек
static const char OUTPUT_PATH[255] = "H:/MyProject/Files/points out.txt";
// путь к файлу ввода точек
static const char INPUT_PATH[255] = "H:/MyProject/Files/points in.txt";
// путь к файлу вывода окружностей
static const char OUTPUT_PATH1[255] = "H:/MyProject/Files/circles out.txt";
// путь к файлу ввода окружностей
static const char INPUT_PATH1[255] = "H:/MyProject/Files/circles in.txt";

// буфер, хранящий координаты последней добавленной вершины
int lastAddPosBuf[2] = {0, 0};
int lastAddPosBufCenter[2] = {0, 0};
int lastAddPosBufPoint[2] = {0, 0};

// динамический список точек
std::vector<Point> points;

// динамический список кругов
std::vector <Circle> circles;

// динамический список прямых
std::vector <Line> lines;

// динамический список отрезков
std::vector <LineSegment> linesegments;

// динамический список точек
std::vector<Point> pointsanswer;

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

// добавить заданное кол-во случайных точек
void randomizePoints(int cnt) {
    for (int i = 0; i < cnt; i++) {
        points.emplace_back(Point::randomPoint());
    }
}
int lastRandoCntBufPoints[1] = {10};

// добавить заданное кол-во случайных окружностей
void randomizeCircles(int cnt) {
    for (int i = 0; i < cnt; i++) {
        circles.emplace_back(Circle::randomCircle());
    }
}
int lastRandoCntBufCircles[1] = {10};

void ShowRandomize() {
    // если не раскрыта панель `Randomize`
    if (!ImGui::CollapsingHeader("Randomize"))
        // заканчиваем выполнение
        return;

    // первый элемент в строке
    ImGui::PushID(0);

    // Инструмент выбора кол-ва точек
    if (ImGui::DragInt("Count", lastRandoCntBufPoints, 0.1, 0, 100)) {

    }
    // восстанавливаем буффер id
    ImGui::PopID();
    // следующий элемент будет на той же строчке
    ImGui::SameLine();
    // второй элемент
    ImGui::PushID(1);
    // создаём кнопку добавления точек
    if (ImGui::Button("Add Points"))
        // по клику добавляем заданное число случайных точек
        randomizePoints(lastRandoCntBufPoints[0]);
    ImGui::PopID();


    //то же, только для прямых
    // первый элемент в строке
    ImGui::PushID(0);

    // Инструмент выбора кол-ва окружностей
    if (ImGui::DragInt("Count", lastRandoCntBufCircles, 0.1, 0, 100)) {

    }
    // восстанавливаем буффер id
    ImGui::PopID();
    // следующий элемент будет на той же строчке
    ImGui::SameLine();
    // второй элемент
    ImGui::PushID(1);
    // создаём кнопку добавления окружностей
    if (ImGui::Button("Add Circles"))
        // по клику добавляем заданное число случайных точек
        randomizeCircles(lastRandoCntBufCircles[0]);
    ImGui::PopID();
}

// загрузка точек из файла
void loadPointsFromFile() {
    // открываем поток данных для чтения из файла
    std::ifstream input(INPUT_PATH);
    // очищаем массив точек
    points.clear();
    // пока не достигнут конец файла
    while (!input.eof()) {
        int x, y;
        input >> x; // читаем x координату
        input >> y; // читаем y координату
        // добавляем в динамический массив точку на основе прочитанных данных
        points.emplace_back(Point(sf::Vector2i(x, y), SET_1));
    }
    // закрываем файл
    input.close();
}

// загрузка кругов из файла
void loadCirclesFromFile() {
    // открываем поток данных для чтения из файла
    std::ifstream input(INPUT_PATH1);
    // очищаем массив точек
    circles.clear();
    // пока не достигнут конец файла
    while (!input.eof()) {
        int x1, y1, x2,y2;
        input >> x1; // читаем x1 координату
        input >> y1; // читаем y1 координату
        input >> x2; // читаем x2 координату
        input >> y2; // читаем y2 координату
        // добавляем в динамический массив точку на основе прочитанных данных
        circles.emplace_back(Circle(Point(sf::Vector2i(x1, y1), SET_1),Point(sf::Vector2i(x2, y2), SET_1), SET_2));
    }
    // закрываем файл
    input.close();
}

// запись в файл точек
void savePointsToFile() {
    // открываем поток данных для записи в файл
    std::ofstream output(OUTPUT_PATH);

    // перебираем точки
    for (auto point: points) {
        // выводим через пробел построчно: x-координату, y-координату и номер множества
        output << point.pos.x << " " << point.pos.y  << std::endl;
    }

    // закрываем
    output.close();
}

// запись в файл точек
void saveCirclesToFile() {
    // открываем поток данных для записи в файл
    std::ofstream output(OUTPUT_PATH1);

    // перебираем точки
    for (auto circle: circles) {
        // выводим через пробел построчно: x-координату, y-координату и номер множества
        output << circle.xpos << " " << circle.ypos <<" "<<circle.rxpos<<" "<<circle.rypos<< std::endl;
    }

    // закрываем
    output.close();
}

// работа с файлами
void ShowFiles() {
    // если не раскрыта панель `Files`
    if (!ImGui::CollapsingHeader("Files"))
        // заканчиваем выполнение
        return;

    // первый элемент в линии
    ImGui::PushID(0);
    // создаём кнопку загрузки
    if (ImGui::Button("Load Points")) {
        // загружаем данные из файла
        loadPointsFromFile();
    }
    // восстанавливаем буфер id
    ImGui::PopID();

    // следующий элемент будет на той же строчке
    ImGui::SameLine();
    // второй элемент
    ImGui::PushID(1);
    // создаём кнопку сохранения
    if (ImGui::Button("Save Points")) {
        // сохраняем задачу в файл
        savePointsToFile();
    }
    // восстанавливаем буфер id
    ImGui::PopID();



    // первый элемент в линии
    ImGui::PushID(0);
    // создаём кнопку загрузки
    if (ImGui::Button("Load Circles")) {
        // загружаем данные из файла
        loadCirclesFromFile();
    }
    // восстанавливаем буфер id
    ImGui::PopID();

    // следующий элемент будет на той же строчке
    ImGui::SameLine();
    // второй элемент
    ImGui::PushID(1);
    // создаём кнопку сохранения
    if (ImGui::Button("Save Circles")) {
        // сохраняем задачу в файл
        saveCirclesToFile();
    }
    // восстанавливаем буфер id
    ImGui::PopID();
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

//решение
void Solve() {
    double rast,maxrast,x1,x2,y1,y2,a1,a2,b1,b2, len, maxlen=0, xans1,xans2,yans1, yans2, x3,y3,x4,y4,a3,a4,b3,b4;
    for (auto A:points){
        for (auto B:points){
            for (auto C:circles){
                x3=A.pos.x;
                y3=A.pos.y;
                x4=B.pos.x;
                y4=B.pos.y;
                a3=C.xpos;
                b3=C.ypos;
                a4=C.rxpos;
                b4=C.rypos;
                if ((x3!=x4)or(y3!=y4))
                {
                    rast = (a3 * (y3 - y4) + b3 * (x4 - x3) + x3 * y4 - y3 * x4) / sqrt((y3-y4) * (y3-y4)+ (x4 - x3) * (x4 - x3));
                    len = (a3 - a4) * (a3 - a4) + (b3 - b4) * (b3 - b4) - rast * rast;
                    if (len > maxlen) {
                        maxlen = len;
                        x1 = x3;
                        x2 = x4;
                        y1 = y3;
                        y2 = y4;
                        a1 = a3;
                        a2 = a4;
                        b1 = b3;
                        b2 = b4;

                    }
                }
            }
        }
    }
    double r=(a1-a2)*(a1-a2)+(b1-b2)*(b1-b2);
    double p=y1-y2; double q=x2-x1; double s=x1*y2-y1*x2;
    //коэффициенты квадратного уравнения
    double k=1+p*p/(q*q);
    double l=-2*a1+2*p*s/(q*q)+2*b1*p/q;
    double m=a1*a1+b1*b1+s*s/(q*q)+2*b1*s/q-r;
    double discriminant=l*l-4*m*k;
    if(discriminant>=0){
        xans1=(-l+pow(discriminant, 0.5))/(2*k);
        yans1=-(p*xans1+s)/q;
        xans2=(-l-pow(discriminant, 0.5))/(2*k);
        yans2=-(p*xans2+s)/q;
    }
    if(maxlen!=0){
        pointsanswer.push_back(Point(sf::Vector2i(xans1, yans1), SET_1));
        pointsanswer.push_back(Point(sf::Vector2i(xans2, yans2), SET_1));
        lines.push_back(Line(Point(sf::Vector2i(xans1, yans1), SET_1),Point(sf::Vector2i(xans2, yans2), SET_1), SET_1));
        linesegments.push_back(LineSegment(Point(sf::Vector2i(xans1, yans1), SET_1),Point(sf::Vector2i(xans2, yans2), SET_1)));}

};


// вывод задачи
void ShowSolve() {
    // если не раскрыта панель `Solve`
    if (!ImGui::CollapsingHeader("Solve"))
        return;
    // первый элемент в линии
    ImGui::PushID(0);
    // создаём кнопку решения
    if (ImGui::Button("Solve")) {
        Solve();
    }

    // восстанавливаем буфер id
    ImGui::PopID();

    // следующий элемент будет на той же строчке
    ImGui::SameLine();
    // второй элемент
    ImGui::PushID(1);

    // создаём кнопку очистки
    if (ImGui::Button("Clear")) {
        // удаляем все точки и окружности
        points.clear();
        circles.clear();
        pointsanswer.clear();
        lines.clear();
        linesegments.clear();
    }
    // восстанавливаем буфер id
    ImGui::PopID();
}




// ручное добавление элементов
void ShowAddElem() {

    // если не раскрыта панель `Add Elem`
    if (!ImGui::CollapsingHeader("Add Elem"))
        // заканчиваем выполнение
        return;



    if (ImGui::DragInt2("Coords", lastAddPosBuf, 0.5f, 0, std::min(WINDOW_SIZE_X, WINDOW_SIZE_Y))) {
        // никаких действий не требуется, достаточно
        // тех изменений буфера, которые imGui выполняет
        // автоматически
    }

    // если нажата кнопка `Point`
    if (ImGui::Button("Point"))
        // добавляем то добавляем в список точку, принадлежащую первому множеству
        points.emplace_back(Point(sf::Vector2i(lastAddPosBuf[0], lastAddPosBuf[1]), SET_1));



    if (ImGui::DragInt2("Coords of center", lastAddPosBufCenter, 0.5f, 0, std::min(WINDOW_SIZE_X, WINDOW_SIZE_Y))) {
        // никаких действий не требуется, достаточно
        // тех изменений буфера, которые imGui выполняет
        // автоматически
    }


    if (ImGui::DragInt2("Coords of point", lastAddPosBufPoint, 0.5f, 0, std::min(WINDOW_SIZE_X, WINDOW_SIZE_Y))) {
        // никаких действий не требуется, достаточно
        // тех изменений буфера, которые imGui выполняет
        // автоматически
    }

    // если нажата кнопка `Circle`
    if (ImGui::Button("Circle"))
        // добавляем то добавляем в список точку, принадлежащую второму множеству
        circles.emplace_back(Circle(Point(sf::Vector2i(lastAddPosBufCenter[0], lastAddPosBufCenter[1]), SET_1),Point(sf::Vector2i(lastAddPosBufPoint[0], lastAddPosBufPoint[1]), SET_1), SET_2));

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

    // перебираем окружности из динамического массива окружностей
    for (auto circle: circles) {
        // добавляем в список рисования окружность
        pDrawList->AddCircle(
                sf::Vector2i(circle.xpos, circle.ypos),
                circle.radius,
                circle.setNum == SET_1 ? ImColor(200, 100, 150) : ImColor(100, 200, 150)
        );
    }



    for (auto point: pointsanswer) {
        // добавляем в список рисования круг
        pDrawList->AddCircleFilled(
                sf::Vector2i(point.pos.x, point.pos.y),
                3,
                ImColor(190, 245, 13),
                20
        );
        // перебираем прямые из динамического массива прямых
        for (auto line: lines) {
            // добавляем в список рисования прямую
            pDrawList->AddLine(
                    sf::Vector2i(line.renderPointA.x, line.renderPointA.y),
                    sf::Vector2i(line.renderPointB.x, line.renderPointB.y),
                    ImColor(0, 0, 255),
                    0.75f
            );
        }
        // перебираем отрезков из динамического массива отрезков
        for (auto linesegment: linesegments) {
            // добавляем в список рисования отезок
            pDrawList->AddLine(
                    sf::Vector2i(linesegment.A1.x, linesegment.A1.y),
                    sf::Vector2i(linesegment.B1.x, linesegment.B1.y),
                    ImColor(255, 0, 0) ,
                    1
            );
        }
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
                    lastAddPosBufCenter[0] = event.mouseButton.x;
                    lastAddPosBufCenter[1] = event.mouseButton.y;
                    lastAddPosBufPoint[0] = event.mouseButton.x;
                    lastAddPosBufPoint[1] = event.mouseButton.y;
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
        // добавление случайных точек
        ShowRandomize();
        // работа с файлами
        ShowFiles();
        // решение задачи
        ShowSolve();
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