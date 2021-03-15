#include <algorithm>
#include <iostream>
#include <assert.h>
#include <vector>
using namespace std;


void test_arr() {
    int array[2][3] = {
        {0, 1, 2},
        {3, 4, 5}
    };
    cout<<&array[0][0]<<" "<<&array[0][1]<<" "<<&array[0][2]<<endl;
    cout<<&array[1][0]<<" "<<&array[1][1]<<" "<<&array[1][2]<<endl;
}

void test_vec() {
    vector<vector<int>> vec = {
        {0, 1, 2},
        {3, 4, 5}
    };
    cout<<&vec[0][0]<<" "<<&vec[0][1]<<" "<<&vec[0][2]<<endl;
    cout<<&vec[1][0]<<" "<<&vec[1][1]<<" "<<&vec[1][2]<<endl;
}

int main() {
    test_arr();     //二维数组是连续一条线
    test_vec();     //二维 vector 是每行一条线
}