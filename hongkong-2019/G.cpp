//
// Created by kskun on 2022/3/31.
//

#include <cassert>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility>

using namespace std;

int K;
int nextNum = 0;
unordered_map<int, int> weights, parents;

int dfsConstruct(int u, int k) {
    if (k == 1) {
        weights[u] = 1;
        return 1;
    }
    if (k == 2) {
        int ch = ++nextNum;
        parents[ch] = u;
        weights[u] = weights[ch] = 1;
        return 1;
    }
    // part of floor(k / 2) solutions
    int rRoot = ++nextNum;
    parents[rRoot] = u;
    int solW = dfsConstruct(rRoot, k / 2);
    // part of 2 solutions
    int lRoot = ++nextNum, lCh = ++nextNum;
    parents[lRoot] = u;
    parents[lCh] = lRoot;
    weights[lRoot] = weights[lCh] = solW;
    // if root should be added
    if (k % 2 == 1) weights[u] = solW * 2;
    else weights[u] = solW * 2 + 1;
    return solW * 2;
}

int main() {
    ios::sync_with_stdio(false);
    cin >> K;
    if (K == 1) {
        cout << "2" << endl << "1" << endl << "2 1" << endl;
        return 0;
    }
    dfsConstruct(++nextNum, K);
    int N = nextNum;
    cout << N << endl;
    for (int i = 2; i <= N; i++) cout << parents[i] << " ";
    cout << endl;
    for (int i = 1; i <= N; i++) cout << weights[i] << " ";
    cout << endl;
    return 0;
}