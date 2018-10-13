

#include "pch.h"

#include <any>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <fstream>
#include <iomanip>
class simplex {
 private:
  bool flag;
  bool flag_optimal;
  bool decision;
  bool max_or_min;
  std::pair<int, int> element;
  std::vector<int> c_;
  std::vector<int> b_;
  std::vector<int> bp;
  std::vector<int> cp;
  std::vector<std::vector<double>> elements;
  std::vector<std::vector<double>> simplex_tabl;
  int rows;
  int columns;
  double perm;

 public:
  simplex() {
    rows = 0;
    columns = 0;
    perm = 0;
    flag = true;
    flag_optimal = true;
    decision = false;
    max_or_min = true;
  };

  void algorithm(const std::string& str) {
    parse_file(str);
    create_simplex_tabl();
    write(std::cout);
    check_decision();
    optimal();
    inv();
    write(std::cout);
    std::cout << "F=" << simplex_tabl[rows][0];
  }
  void check_decision() {
    bool not_minus = true;
    for (int i = 0; i < rows && not_minus; ++i) {
      if (simplex_tabl[i][0] < 0) {
        for (int j = 1; j < columns + 1; ++j) {
          if (simplex_tabl[i][j] > 0) {
            flag = false;

          } else {
            flag = true;
            decision = true;
            not_minus = false;
            element.first = i;
            element.second = j;
            break;
          }
        }
      } else
        continue;
    }
  }
  void create_simplex_tabl() {
    simplex_tabl.resize(rows + 1);
    for (int i = 0; i < rows + 1; ++i) {
      simplex_tabl[i].resize(columns + 1);
      for (int j = 0; j < columns + 1; ++j) {
        simplex_tabl[i][j] = elements[i][j];
      }
    }
    bp.resize(rows);
    cp.resize(rows);
    for (int i = 0; i < rows; ++i) {
      bp[i] = (i + 4);
      cp[i] = (i + 1);
    }
  }
  void optimal() {
    if (flag_optimal) {
      for (int i = 0; i < rows + 1; ++i) {
        for (int j = 0; j < columns + 1; ++j) {
          elements[i][j] = simplex_tabl[i][j];
        }
      }
      if (flag) {
        //разрещающий элемент;
        if (decision)
          decision = false;
        else
          element = rows_and_columns(element);
        //заменив базис поменяв переменные местами

        change_bazis(element);

        for (int i = 1; i < columns + 1; ++i) {
          if (simplex_tabl[rows][i] > 0) {
            flag_optimal = true;
            break;
          } else {
            flag_optimal = false;
          }
        }
        write(std::cout);
        optimal();
      } else
        std::cout << "there is no decision";
    } else
      std::cout << " Optimal decision";
  }
  void change_bazis(std::pair<int, int>& element) {
    //изменение столбца и строки
    std::swap(cp[element.second - 1], bp[element.first]);
    for (int i = 0; i < rows + 1; ++i) {
      if (i == element.first) {
        simplex_tabl[i][element.second] =
            1 / elements[element.first][element.second];
      } else
        simplex_tabl[i][element.second] /=
            -elements[element.first][element.second];
    }

    for (int i = 0; i < columns + 1; ++i) {
      if (i == element.second)
        ;
      else
        simplex_tabl[element.first][i] /=
            elements[element.first][element.second];
    }

    for (int i = 0; i < rows + 1; ++i) {
      for (int j = 0; j < columns + 1; ++j) {
        if (i != element.first && j != element.second) {
          simplex_tabl[i][j] =
              elements[i][j] -
              ((elements[i][element.second] * elements[element.first][j]) /
               (elements[element.first][element.second]));
        }
      }
    }
  }
  std::pair<int, int>& rows_and_columns(std::pair<int, int>& element) {
    //находим наибольший столбец
    int k = 1;
    for (int j = 1; j < columns; ++j) {
      if (simplex_tabl[rows][k] < simplex_tabl[rows][j + 1]) k = j + 1;
    }
    element.second = k;
    // находим разрещающий строку
    int q = 0;
    for (int i = 0; i < rows - 1; ++i) {
      if ((simplex_tabl[i][0] / simplex_tabl[q][element.second]) >
          (simplex_tabl[i + 1][0] / simplex_tabl[i + 1][element.second]))
        q = i + 1;
    }
    element.first = q;
    return element;
  }
  std::ostream& write(std::ostream& stream) {
    stream << std::endl;
    stream << std::setw(14) << "S";
    for (int i = 0; i < rows; ++i) {
      stream << std::setw(9) << "X" << cp[i];
    }
    stream << std::endl;
    for (int i = 0; i < rows + 1; ++i) {
      if (i < rows)
        stream << std::setw(3) << "X" << bp[i];
      else
        stream << std::setw(4) << "F";
      for (int j = 0; j < columns + 1; ++j) {
        if (simplex_tabl[i][j] == int(simplex_tabl[i][j])) {
          stream << std::setw(10) << simplex_tabl[i][j];

        } else {
          stream << std::setw(10) << std::setprecision(3) << simplex_tabl[i][j];
        }
      }
      stream << std::endl << std::endl;
    }
    return stream;
  }
  void parse_file(const std::string& str) {
    char op;
    std::ifstream stream;
    stream.open(str.c_str());
    if (stream.is_open()) {
      if (stream >> op && op == 'c' && stream >> op && op == '=') {
        bool flag = true;
        stream >> op;
        if (op == '(') {
          int Num;
          stream >> op;
          while (flag) {
            if (isdigit(op)) {
              Num = op - '0';
              c_.push_back(Num);
              ++rows;
              stream >> op;
            } else {
              flag = false;
              break;
            }
          }
        }
      }
      if (stream >> op && op == 'b' && stream >> op && op == '=') {
        bool flag = true;
        stream >> op;
        if (op == '(') {
          int Num;
          stream >> op;
          while (flag) {
            if (isdigit(op)) {
              Num = op - '0';
              b_.push_back(Num);
              ++columns;
              stream >> op;
            } else {
              flag = false;
              break;
            }
          }
        }
      }
      if (stream >> op && op == 'a' && stream >> op && op == '=') {
        double Num;
        int k = 0, l = 0;
        stream >> Num;
        elements.resize(rows + 1);
        for (int i = 0; i < rows + 1; ++i) {
          elements[i].resize(columns + 1);
          for (int j = 0; j < columns + 1; ++j) {
            if (j == 0) {
              if (i == rows) {
                elements[i][j] = 0;
              } else {
                elements[i][j] = b_[k];
                ++k;
              }

            } else if (j > 0 && i == rows) {
              elements[i][j] = c_[l];
              ++l;
            } else {
              elements[i][j] = Num;
              stream >> Num;
            }
          }
        }
      }
      stream.clear();
    }
  }
  void inv() {
    if (!(max_or_min)) simplex_tabl[rows][0] *= -1;
  }
  void max_or_min1(bool& q) {
    if (!(q)) max_or_min = false;
  }
};
int main() {
  simplex S;
  char op;
  std::string str;
  std::string m_or_m;
  std::cout << "Cin path file" << std::endl;
  std::cin >> str;
  std::cout << " max or min?" << std::endl;
  std::cin >> m_or_m;
  bool m_m = true;
  std::istringstream stream(m_or_m);
  std::string str2;
  while (stream >> op) {
    str2.push_back(op);
  }
  if (str2 == "min")
    m_m = true;
  else if (str2 == "max")
    m_m = false;
  S.max_or_min1(m_m);
  S.algorithm(str);
}
