g++-15 -std=c++23 *.cpp -o opt_lvl0.out -O0
g++-15 -std=c++23 *.cpp -o opt_lvl1.out -O1
g++-15 -std=c++23 *.cpp -o opt_lvl2.out -O2
g++-15 -std=c++23 *.cpp -o opt_lvl3.out -O3
g++-15 -std=c++23 *.cpp -o opt_lvl3_native.out -O3 -march=native
