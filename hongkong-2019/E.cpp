//
// Created by KSkun on 2022/3/30.
//
#include <iostream>

using namespace std;

const int MAXN = 5005;

int N, a[MAXN];

bool checkSurvive(int index) {
    // larger than a[index]: N - a[index]
    // smaller than a[index]: a[index] - 1
    int diffLarSma = N - a[index] - (a[index] - 1);
    bool moreLar = (diffLarSma > 0);
    diffLarSma = abs(diffLarSma);
    int countCont = 0;
    for (int i = 1; i <= N; i++) {
        if (i == index) {
            countCont = 0;
            continue;
        }
        if (moreLar) {
            // swipe larger numbers
            // if pattern 11011 exists, erase 01 to construct a 111 pattern
            if (a[i] > a[index]) countCont++;
            else countCont = max(countCont - 1, 0);
            if (countCont == 3) {
                countCont = 1;
                diffLarSma -= 2;
            }
        } else {
            // swipe smaller numbers
            if (a[i] < a[index]) countCont++;
            else countCont = max(countCont - 1, 0);
            if (countCont == 3) {
                countCont = 1;
                diffLarSma -= 2;
            }
        }
    }
    return diffLarSma <= 0;
}

int main() {
    ios::sync_with_stdio(false);
    int T;
    cin >> T;
    while (T--) {
        cin >> N;
        for (int i = 1; i <= N; i++) {
            cin >> a[i];
        }
        for (int i = 1; i <= N; i++) {
            if (checkSurvive(i)) cout << 1;
            else cout << 0;
        }
        cout << endl;
    }
    return 0;
}
