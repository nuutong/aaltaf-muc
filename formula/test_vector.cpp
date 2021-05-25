// #include "formula/aalta_formula.h"
// #include "ltlfchecker.h"
// #include "carchecker.h"
//#include "solver.h"
#include <stdio.h>
#include <string.h>
#include<vector>
#include<iostream>
using namespace std;

int main(){
    vector<int> a={0,1,2,6};
    vector<int> b ={3};
    vector<int>e;
     vector<vector<int>> c;
    // b.insert(b.end(),a.begin(),a.begin()+2);
    // b.insert(b.end(),a.begin()+2,a.end());
    for(auto it=b.begin();it!=b.end();++it){
        cout<<*it<<endl;
    }

 return 0;
}

