#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>
#include <fstream>

class Taylor {
    std::vector<double> coefficients;
    double a_param = 0.0;

public:
    void setNumTerms(int n) {
        coefficients.assign(n, 0.0);
    }

    int getNumTerms() const {
        return static_cast<int>(coefficients.size());
    }

    // f^(n)(a) для exp(x) = exp(a)
    void computeCoefficients(double a) {
        a_param = a;
        if (coefficients.empty()) return;

        double term = std::exp(a);
        coefficients[0] = term;
        for (size_t n = 1; n < coefficients.size(); ++n) {
            term /= static_cast<double>(n);
            coefficients[n] = term;
        }
    }

    void printFormula() const {
        std::cout << "f(x) = ";
        for (size_t n = 0; n < coefficients.size(); ++n) {
            std::cout << std::fixed << std::setprecision(4) << coefficients[n]
                      << "*(x - " << a_param << ")^" << n;
            if (n + 1 < coefficients.size()) std::cout << " + ";
        }
        std::cout << '\n';
    }

    double getTermValue(int n, double x) const {
        if (n < 0 || n >= getNumTerms()) return 0.0;
        return coefficients[n] * std::pow(x - a_param, n);
    }

    double calculateAt(double x) const {
        double sum = 0.0, power = 1.0;
        const double dx = x - a_param;
        for (double c : coefficients) {
            sum += c * power;
            power *= dx;
        }
        return sum;
    }

    void printDeviation(double x) const {
        const double taylor_val = calculateAt(x);
        const double std_val = std::exp(x);
        std::cout << "Точка x: " << x << "\n"
                  << "Значение ряда: " << taylor_val << "\n"
                  << "Эталон (cmath): " << std_val << "\n"
                  << "Отклонение: " << std::abs(taylor_val - std_val) << "\n";
    }

    friend std::ostream& operator<<(std::ostream& os, const Taylor& obj) {
        os << obj.coefficients.size() << ' ' << obj.a_param << ' ';
        for (double c : obj.coefficients) os << c << ' ';
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Taylor& obj) {
        int n;
        double a;
        if (is >> n >> a) {
            obj.setNumTerms(n);
            obj.a_param = a;
            for (double& c : obj.coefficients) is >> c;
        }
        return is;
    }
};

int main() {
    Taylor expSeries;

    int n;
    double a, x;

    std::cout << "Введите количество членов ряда: ";
    std::cin >> n;
    expSeries.setNumTerms(n);

    std::cout << "Введите параметр a: ";
    std::cin >> a;
    expSeries.computeCoefficients(a);

    std::cout << "\nФормула ряда:\n";
    expSeries.printFormula();

    std::cout << "\nВведите точку x для расчета: ";
    std::cin >> x;
    expSeries.printDeviation(x);

    std::ofstream("taylor.txt") << expSeries;
    std::cout << "\nДанные ряда сохранены в taylor.txt\n";

    return 0;
}
