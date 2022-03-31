//
// Created by kskun on 2022/3/31.
//

#include <cassert>

#include <iostream>
#include <utility>
#include <tuple>
#include <vector>
#include <array>
#include <algorithm>
#include <list>

using namespace std;

struct SoldierInfo {
    int x, h, time;
};

struct EnemyInfo {
    int L, R, H, time, ansNum;
};

const int MAXN = 2e6 + 5;

int N, M, soldierNum, enemyNum;
vector<SoldierInfo> soldiers;
vector<EnemyInfo> enemies;

bool compareSoldierWithX(const SoldierInfo &a, const SoldierInfo &b) {
    return a.x < b.x;
}

bool compareEnemyWithR(const EnemyInfo &a, const EnemyInfo &b) {
    return a.R < b.R;
}

// store values
// insert h values in ascending order of position (x)
struct SegmentTree {
    int size;

    struct Info {
        int pos, time;
    };

    struct Node {
        vector<Info> monoStack;

        void insert(const Info &info) {
            while (!monoStack.empty() && monoStack.rbegin()->time > info.time) monoStack.pop_back();
            monoStack.push_back(info);
        }

        static bool compareInfoWithPosition(const Info &a, const Info &b) {
            return a.pos < b.pos;
        }

        bool hasValidInfo(int L, int time) {
            auto it = lower_bound(monoStack.begin(), monoStack.end(), Info{L, 0},
                                  compareInfoWithPosition);
            return it != monoStack.end() && it->time <= time;
        }
    } *data;

    explicit SegmentTree(int size = MAXN) : size(size) {
        data = new Node[4 * size + 5];
    }

    ~SegmentTree() {
        delete[] data;
    }

    void insert(int x, int h, int time) {
        int idx = 1, inL = 1, inR = size;
        while (true) {
            data[idx].insert({x, time});
            if (inL == inR) break;
            int mid = (inL + inR) / 2;
            if (h <= mid) {
                inR = mid;
                idx = idx * 2; // left child
            } else {
                inL = mid + 1;
                idx = idx * 2 + 1; // right child
            }
        }
    }

    void insert(const SoldierInfo &info) {
        auto[x, h, time] = info;
        insert(x, h, time);
    }

    int recurQueryLessEq(int idx, int inL, int inR, int L, int H, int time) {
        int mid = (inL + inR) / 2;
        if (inL >= 1 && inR <= H) {
            if (data[idx].hasValidInfo(L, time)) {
                if (inL == inR) return inL;
                auto rRes = recurQueryLessEq(idx * 2 + 1, mid + 1, inR, L, H, time);
                if (rRes != -1) return rRes;
                else return recurQueryLessEq(idx * 2, inL, mid, L, H, time);
            } else {
                // cut branch
                return -1;
            }
        } else {
            if (data[idx].hasValidInfo(L, time)) {
                int rRes = -1;
                if (H > mid) rRes = recurQueryLessEq(idx * 2 + 1, mid + 1, inR, L, H, time);
                if (rRes != -1) return rRes;
                else return recurQueryLessEq(idx * 2, inL, mid, L, H, time);
            } else {
                // cut branch
                return -1;
            }
        }
    }

    int queryLessEq(int L, int H, int time) {
        // query [1, H]
        return recurQueryLessEq(1, 1, size, L, H, time);
    }

    int recurQueryGreater(int idx, int inL, int inR, int L, int H, int time) {
        int mid = (inL + inR) / 2;
        if (inL > H) {
            if (data[idx].hasValidInfo(L, time)) {
                if (inL == inR) return inL;
                auto lRes = recurQueryGreater(idx * 2, inL, mid, L, H, time);
                if (lRes != -1) return lRes;
                else return recurQueryGreater(idx * 2 + 1, mid + 1, inR, L, H, time);
            } else {
                // cut branch
                return -1;
            }
        } else {
            if (data[idx].hasValidInfo(L, time)) {
                int lRes = -1;
                if (H < mid) lRes = recurQueryGreater(idx * 2, inL, mid, L, H, time);
                if (lRes != -1) return lRes;
                else return recurQueryGreater(idx * 2 + 1, mid + 1, inR, L, H, time);
            } else {
                // cut branch
                return -1;
            }
        }
    }

    int queryGreater(int L, int H, int time) {
        // query [H+1, size]
        return recurQueryGreater(1, 1, size, L, H, time);
    }
};

int getNormalizedValue(const vector<int> &values, int value) {
    return lower_bound(values.begin(), values.end(), value) - values.begin() + 1;
}

int getRealValue(const vector<int> &values, int normalized) {
    assert(normalized >= 1 && normalized <= values.size());
    return values[normalized - 1];
}

void prepareNormalization(vector<int> &values) {
    values.clear();
    values.reserve(soldiers.size() + enemies.size() + 2);
    values.push_back(0); // min
    values.push_back(1e9 + 5); // max
    for (auto &soldier: soldiers) {
        values.push_back(soldier.h);
    }
    for (auto &enemy: enemies) {
        values.push_back(enemy.H);
    }
    sort(values.begin(), values.end());
    values.erase(unique(values.begin(), values.end()), values.end());
    for (auto &soldier: soldiers) {
        soldier.h = getNormalizedValue(values, soldier.h);
    }
    for (auto &enemy: enemies) {
        enemy.H = getNormalizedValue(values, enemy.H);
    }
}

int answers[MAXN], ansNum;

int main() {
    int T;
    scanf("%d", &T);
    while (T--) {
        scanf("%d%d", &N, &M);
        soldiers.clear();
        enemies.clear();
        ansNum = 0;
        for (int i = 1; i <= M; i++) {
            int type;
            scanf("%d", &type);
            if (type == 0) {
                int x, h;
                scanf("%d%d", &x, &h);
                soldiers.push_back({x, h, i});
            } else {
                int L, R, H;
                scanf("%d%d%d", &L, &R, &H);
                enemies.push_back({L, R, H, i, ++ansNum});
            }
        }
        soldierNum = soldiers.size();
        enemyNum = enemies.size();
        vector<int> values;
        prepareNormalization(values);

        sort(soldiers.begin(), soldiers.end(), compareSoldierWithX);
        sort(enemies.begin(), enemies.end(), compareEnemyWithR);
        int soldierIdx = 0, enemyIdx = 0;
        SegmentTree tr(values.size() + 5);
        while (enemyIdx < enemyNum) {
            int enemyIdxN = enemyIdx;
            while (enemyIdxN < enemyNum && enemies[enemyIdxN].R == enemies[enemyIdx].R) enemyIdxN++;
            while (soldierIdx < soldierNum && soldiers[soldierIdx].x <= enemies[enemyIdx].R) {
                tr.insert(soldiers[soldierIdx]);
                soldierIdx++;
            }
            for (int i = enemyIdx; i < enemyIdxN; i++) {
                auto &enemy = enemies[i];
                auto realH = getRealValue(values, enemy.H);
                int ans = -1, res;
                res = tr.queryLessEq(enemy.L, enemy.H, enemy.time);
                if (res != -1) {
                    auto realVal = getRealValue(values, res);
                    auto val = realH - realVal;
                    if (ans == -1 || val < ans) ans = val;
                }
                res = tr.queryGreater(enemy.L, enemy.H, enemy.time);
                if (res != -1) {
                    auto realVal = getRealValue(values, res);
                    auto val = realVal - realH;
                    if (ans == -1 || val < ans) ans = val;
                }
                answers[enemy.ansNum] = ans;
            }
            enemyIdx = enemyIdxN;
        }
        for (int i = 1; i <= ansNum; i++) {
            printf("%d\n", answers[i]);
        }
    }
    return 0;
}