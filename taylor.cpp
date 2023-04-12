#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <math.h>

#include "eigen-3.4/Eigen/Core"
#include "eigen-3.4/Eigen/LU"

#include <cstdio>
#include <iomanip>

int Countnumofline(std::string file)
{
  std::ifstream ifs(file); // ファイルオープン
  int numofnode = 0;       // 行の数を数える定数
  std::string str;         // 文字列指定
  while (std::getline(ifs, str))
  {
    numofnode++;
  }
  ifs.close();
  return numofnode;
}

int main(int argc, char *argv[])
{
  int numofnode = Countnumofline("example/node_d.dat");
  int numofele = Countnumofline("example/element_d.dat");

  std::vector<std::vector<double>> node(numofnode, std::vector<double>(3, 0));
  std::ifstream ifs("example/node_d.dat"); // ファイル入力
  for (int i = 0; i < numofnode; i++)
  {
    std::vector<double> node_t(3, 0);
    for (int j = 0; j < 3; j++)
    {
      ifs >> node_t[j];
    }
    node[i] = node_t;
  }
  ifs.close();

  std::vector<std::vector<double>> element(numofele, std::vector<double>(3, 0));
  std::fstream ifs1("example/element_d.dat"); // ファイル入力
  for (int i = 0; i < numofele; i++)
  {
    std::vector<double> element_t(2, 0);
    for (int j = 0; j < 2; j++)
    {
      ifs1 >> element_t[j];
    }
    element[i] = element_t;
  }
  ifs1.close();

  std::vector<double> r(numofele, 0.0);
  std::ifstream ifs2("example/rad_d.dat"); // ファイル入力
  for (int i = 0; i < numofele; i++)
  {
    ifs2 >> r[i];
  }
  ifs2.close();

  std::vector<double> u(numofele, 0.0);

  for (int i = 0; i < numofele; i++)
  {
    u[i] = 20.0e0; // mm
  }

  using namespace Eigen;
  int N = numofele, M = 1200;
  double dt = 5.0e-2; // 時間刻み
  const double PI = acos(-1);

  std::vector<std::vector<double>> f(M + 1, std::vector<double>(numofnode, 0.0));
  std::vector<double> contrast(numofnode, 0.0);
  std::vector<double> Ae(numofele, 0.0);
  std::vector<std::vector<std::vector<double>>> xx(2, std::vector<std::vector<double>>(3, std::vector<double>(numofele, 0.0)));
  std::vector<double> L(numofele, 0.0);

  // initial condition
  for (int i = 0; i < numofnode; i++)
  {
    if ((i >= 1) && (i <= 11))
    {
      f[0][i] = 1.0;
    }
    else
    {
      f[0][i] = 0.0;
    }
  }

  for (int i = 0; i < numofele; i++)
  {
    for (int p = 0; p < 2; p++)
    { // 一次元なので両端の二点
      for (int q = 0; q < 3; q++)
      {
        xx[p][q][i] = node[element[i][p]][q]; // xx[node番号][xかy]それぞれのnodeの座標
      }
    }
    L[i] = 1.0e3 * (sqrt(pow((xx[1][0][i] - xx[0][0][i]), 2.0) + pow((xx[1][1][i] - xx[0][1][i]), 2.0) + pow((xx[1][2][i] - xx[0][2][i]), 2.0))); // mからmm
    Ae[i] = PI * r[i] * r[i] * 1.0e6;
  }

  std::ofstream ofs4("example/L.dat");
  for (int i = 0; i < numofele; i++)
  {
    ofs4 << L[i] << std::endl;
  }
  ofs4.close();

  for (int i = 0; i < M; i++)
  {
    Eigen::MatrixXd A = MatrixXd::Zero(numofnode, numofnode);
    Eigen::VectorXd b = VectorXd::Zero(numofnode);
    for (int j = 0; j < numofele; j++)
    {
      int ele0 = element[j][0];
      int ele1 = element[j][1];

      A(ele0, ele0) = A(ele0, ele0) + 2.0e0 * L[j] / (dt * 6.0e0);
      A(ele0, ele1) = A(ele0, ele1) + 1.0e0 * L[j] / (dt * 6.0e0);
      A(ele1, ele0) = A(ele1, ele0) + 1.0e0 * L[j] / (dt * 6.0e0);
      A(ele1, ele1) = A(ele1, ele1) + 2.0e0 * L[j] / (dt * 6.0e0);
    }
    for (int j = 0; j < numofele; j++)
    {
      int ele0 = element[j][0];
      int ele1 = element[j][1];
      double f1 = f[i][ele0], f2 = f[i][ele1];

      b(ele0) = b(ele0) + L[j] / (6.0e0 * dt) * (2.0e0 * f1 + 1.0e0 * f2);
      b(ele1) = b(ele1) + L[j] / (6.0e0 * dt) * (1.0e0 * f1 + 2.0e0 * f2);

      b(ele0) = b(ele0) - u[j] / 2.0e0 * (-f1 + f2);
      b(ele1) = b(ele1) - u[j] / 2.0e0 * (-f1 + f2);

      b(ele0) = b(ele0) - dt * u[j] * u[j] / (2.0e0 * L[j]) * (f1 - f2);
      b(ele1) = b(ele1) - dt * u[j] * u[j] / (2.0e0 * L[j]) * (-f1 + f2);
    }

    for (int j = 0; j < N + 1; j++)
    {
      A(0, j) = 0.0e0;
    }
    A(0, 0) = 1.0e0;
    b(0) = 0.0e0;

    Eigen::VectorXd x = VectorXd::Zero(N + 1);
    x = A.fullPivLu().solve(b); // 行列計算
    for (int j = 0; j < x.size(); j++)
    {
      f[i + 1][j] = x(j);
    }
  }

  int d = 100;
  // int i=100;//見たい時刻t=0~200
  std::ofstream ofs("example/out_taylor.dat");
  for (int i = 0; i < 7; i++)
  {
    for (int j = 0; j < d + 1; j++)
    {
      ofs << f[i * 200][j] << " ";
    }
    ofs << std::endl;
  }
  ofs.close();

  std::ofstream ofs1("example/all_taylor.dat");
  for (int i = 0; i < M + 1; i++)
  {
    for (int j = 0; j < d + 1; j++)
    {
      ofs1 << f[i][j] << " ";
    }
    ofs1 << std::endl;
  }
  ofs1.close();

  std::vector<std::vector<int>> origin(120, std::vector<int>(numofnode, 0));

  for (int i = 0; i < 120; i++)
  {
    for (int j = 0; j < numofnode; j++)
    {
      if ((i + 11) >= j && (i + 1) <= j)
      {
        origin[i][j] = 1;
      }
    }
  }

  std::ofstream ofs2("example/all_origin.dat");
  for (int i = 0; i < 120; i++)
  {
    for (int j = 0; j < numofnode; j++)
    {
      ofs2 << origin[i][j] << " ";
    }
    ofs2 << std::endl;
  }
  ofs2.close();

  return 0;
}