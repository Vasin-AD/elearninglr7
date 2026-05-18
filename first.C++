#include <iostream>
#include <cmath>
#include <stdexcept>
#include <utility>

// Тип указателя на функцию одной переменной
using FuncPtr = double(*)(double);

class Tabulator {
private:
    double a;          // Начало интервала
    double b;          // Конец интервала
    int n;             // Количество точек
    double* x_arr;     // Динамический массив X
    double* f_arr;     // Динамический массив F
    FuncPtr current_f; // Указатель на текущую табулируемую функцию

    // Очистка выделенной памяти
    void clear_arrays() {
        delete[] x_arr;
        delete[] f_arr;
        x_arr = nullptr;
        f_arr = nullptr;
    }

public:
    // Конструктор по умолчанию
    Tabulator() : a(0.0), b(0.0), n(0), x_arr(nullptr), f_arr(nullptr), current_f(nullptr) {}

    // Параметризованный конструктор
    Tabulator(double start, double end, int count) 
        : a(start), b(end), n(count), x_arr(nullptr), f_arr(nullptr), current_f(nullptr) {
        if (n < 2 && n != 0) throw std::invalid_argument("Количество точек должно быть >= 2");
    }

    // Деструктор
    ~Tabulator() {
        clear_arrays();
    }

    // 1) Задать интервал табулирования
    void set_interval(double start, double end) {
        a = start;
        b = end;
        clear_arrays(); // Сбрасываем старые данные при изменении параметров
    }

    // 2) Задать число точек табулирования
    void set_points_count(int count) {
        if (count < 2) throw std::invalid_argument("Количество точек должно быть >= 2");
        n = count;
        clear_arrays();
    }

    // 3) Узнать интервал табулирования
    std::pair<double, double> get_interval() const {
        return {a, b};
    }

    // 4) Узнать число точек табулирования
    int get_points_count() const {
        return n;
    }

    // 5) Выполнить табулирование функции
    void tabulate(FuncPtr f) {
        if (n < 2) throw std::runtime_error("Не задано корректное количество точек (n >= 2)");
        if (f == nullptr) throw std::invalid_argument("Указатель на функцию не может быть нулевым");

        clear_arrays();
        current_f = f;

        x_arr = new double[n];
        f_arr = new double[n];

        double h = (b - a) / (n - 1);

        for (int i = 0; i < n; ++i) {
            x_arr[i] = a + i * h;
            f_arr[i] = current_f(x_arr[i]);
        }
    }

    // 6) Вычислить значение методом линейной интерполяции
    double interpolate(double x) const {
        if (x_arr == nullptr || f_arr == nullptr) {
            throw std::runtime_error("Табулирование еще не было выполнено");
        }
        if (x < a || x > b) {
            throw std::out_of_range("Точка x находится вне интервала табулирования [a, b]");
        }

        // Поиск интервала [x_i, x_{i+1}]
        int i = 0;
        for (int j = 0; j < n - 1; ++j) {
            if (x >= x_arr[j] && x <= x_arr[j + 1]) {
                i = j;
                break;
            }
        }

        // Формула линейной интерполяции
        double x1 = x_arr[i], x2 = x_arr[i + 1];
        double y1 = f_arr[i], y2 = f_arr[i + 1];
        
        return y1 + ((x - x1) * (y2 - y1)) / (x2 - x1);
    }

    // Сравнить интерполированное значение с истинным
    void compare_values(double x) const {
        if (current_f == nullptr) {
            throw std::runtime_error("Исходная функция не задана (вызовите tabulate)");
        }

        double approx = interpolate(x);
        double exact = current_f(x);
        double diff = std::abs(approx - exact);

        std::cout << "Точка x: " << x << "\n";
        std::cout << "Интерполированное значение: " << approx << "\n";
        std::cout << "Точное значение функции:   " << exact << "\n";
        std::cout << "Абсолютная погрешность:    " << diff << "\n";
    }
};

// Тестовые функции для табулирования
double func_sin(double x) { return std::sin(x); }
double func_cos(double x) { return std::cos(x); }
double func_exp(double x) { return std::exp(x); }
double func_square(double x) { return x * x; }
double func_cube(double x) { return x * x * x; }
double func_sqrt(double x) {
    if (x < 0) throw std::domain_error("sqrt(x) определён только при x >= 0");
    return std::sqrt(x);
}

struct FunctionOption {
    const char* name;
    FuncPtr func;
};

FuncPtr select_function() {
    const FunctionOption options[] = {
        {"sin(x)",   func_sin},
        {"cos(x)",   func_cos},
        {"exp(x)",   func_exp},
        {"x^2",      func_square},
        {"x^3",      func_cube},
        {"sqrt(x)",  func_sqrt},
    };
    const int count = sizeof(options) / sizeof(options[0]);

    std::cout << "Выберите функцию для табулирования:\n";
    for (int i = 0; i < count; ++i) {
        std::cout << "  " << (i + 1) << ") " << options[i].name << "\n";
    }

    int choice = 0;
    std::cout << "Номер функции (1-" << count << "): ";
    if (!(std::cin >> choice) || choice < 1 || choice > count) {
        throw std::invalid_argument("Некорректный выбор функции");
    }

    std::cout << "Выбрана функция: " << options[choice - 1].name << "\n\n";
    return options[choice - 1].func;
}

std::pair<double, double> select_interval() {
    double a = 0.0, b = 0.0;

    std::cout << "Задайте границы интервала табулирования:\n";
    std::cout << "  a (начало): ";
    if (!(std::cin >> a)) {
        throw std::invalid_argument("Некорректное значение начала интервала");
    }
    std::cout << "  b (конец):  ";
    if (!(std::cin >> b)) {
        throw std::invalid_argument("Некорректное значение конца интервала");
    }
    if (a >= b) {
        throw std::invalid_argument("Начало интервала должно быть меньше конца (a < b)");
    }

    std::cout << "Интервал задан: [" << a << ", " << b << "]\n\n";
    return {a, b};
}

int main() {
    try {
        Tabulator tab;

        FuncPtr selected = select_function();
        auto [a, b] = select_interval();

        tab.set_interval(a, b);
        tab.set_points_count(5);

        auto [start, end] = tab.get_interval();
        std::cout << "Интервал: [" << start << ", " << end << "]\n";
        std::cout << "Количество точек: " << tab.get_points_count() << "\n\n";

        tab.tabulate(selected);
        std::cout << "Табулирование выполнено.\n\n";

        double test_point = (a + b) / 2.0;
        tab.compare_values(test_point);

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}