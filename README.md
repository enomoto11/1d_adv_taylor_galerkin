# 1d_adv on C++

## Eigenを使用して０次元血流解析と1次元移流方程式を解く．
### できること
- $\rm{Ax=b}$を解き,$\rm{x}$を返す（これが基本）


## HOW TO USE THIS

g++ -std=c++14　-o run taylor.cpp -isystem /usr/include/eigen3　

## 二分岐血管においての仕組み
divide_ori.cppで１つの要素を２０分割する⇒1d_Qiita.cppで一次元移流方程式を解く⇒vtkファイルに出力