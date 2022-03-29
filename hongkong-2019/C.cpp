//
// Created by KSkun on 2022/3/29.
//
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

typedef long long LL;

const int MAXN = 2e5 + 5;

class FenwickTree {
public:
    int n, *data;

    static inline int lowbit(int x) {
        return x & -x;
    }

    explicit FenwickTree(int n = MAXN) : n(n) {
        data = new int[n];
        fill(data, data + n, 0);
    }

    ~FenwickTree() {
        delete[] data;
    }

    void add(int index, int toAdd) {
        for (; index < n; index += lowbit(index)) {
            data[index] += toAdd;
        }
    }

    int querySum(int right) const {
        int sum = 0;
        for (; right > 0; right -= lowbit(right)) {
            sum += data[right];
        }
        return sum;
    }
};

int n, a[MAXN];
vector<int> G[MAXN];
LL ans;

int cent, allSize, subtSize[MAXN], maxChSize[MAXN];
bool isCent[MAXN];

void dfsFindCent(int u, int dfsPar) {
    subtSize[u] = 1;
    maxChSize[u] = 0;
    for (auto v : G[u]) {
        if (isCent[v] || v == dfsPar) continue;
        dfsFindCent(v, u);
        subtSize[u] += subtSize[v];
        maxChSize[u] = max(maxChSize[u], subtSize[v]);
    }
    maxChSize[u] = max(maxChSize[u], allSize - subtSize[u]);
    if (maxChSize[u] < maxChSize[cent]) cent = u;
}

int findCent(int subtRoot) {
    cent = 0;
    allSize = subtSize[subtRoot];
    if (subtRoot == 1) allSize = n; // 第一次求重心
    maxChSize[0] = 1e9;
    dfsFindCent(subtRoot, 0);
    return cent;
}

typedef tuple<int, LL, int> ChainInfo; // max, sum, root of subtree

void dfsChainSum(int u, LL sumW, int maxW, int subtRoot, int dfsPar, vector<ChainInfo> &infos) {
    sumW += a[u];
    maxW = max(maxW, a[u]);
    infos.emplace_back(maxW, sumW, subtRoot);
    for (auto v : G[u]) {
        if (isCent[v] || v == dfsPar) continue;
        dfsChainSum(v, sumW, maxW, subtRoot, u, infos);
    }
}

int queryNormalized(const vector<LL> &tmpValues, LL value) {
    return lower_bound(tmpValues.begin(), tmpValues.end(), value) - tmpValues.begin() + 1;
}

void prepareNormalization(vector<ChainInfo> &values, vector<LL> &tmpValues) {
    tmpValues.clear();
    tmpValues.reserve(values.size());
    for (auto[maxW, sumW, subtRoot] : values) {
        tmpValues.push_back(sumW);
    }
    sort(tmpValues.begin(), tmpValues.end());
    tmpValues.erase(unique(tmpValues.begin(), tmpValues.end()), tmpValues.end());
//    for (auto &v : values) {
//        v.second = queryNormalized(tmpValues, v.second);
//    }
}

// fixed chainSum[u -> cent]
// count v that chainSum[u -> v] > 2 chainMax[u -> cent]
//   that is chainSum[v -> cent] > 2 chainMax[u -> cent] - chainSum[u -> cent] + weight[cent]
// with chainMax[u -> v] == chainMax[u -> cent]
//   that is chainMax[u -> cent] >= chainMax[v -> cent]
//   implemented with rising chainMax[u] order
LL countInInfos(vector<ChainInfo> &infos, vector<LL> &tmpSumValues, int u) {
    FenwickTree tr(tmpSumValues.size() + 5);
    int valuesInTr = 0;
    LL res = 0;
    sort(infos.begin(), infos.end());
    for (auto[maxW, sumW, subtRoot] : infos) {
        LL lessThan = 2 * maxW - sumW + a[u];
        int queryResult = tr.querySum(queryNormalized(tmpSumValues, lessThan));
        int largerCount = valuesInTr - queryResult;
        res += largerCount;
        valuesInTr++;
        tr.add(queryNormalized(tmpSumValues, sumW), 1);
    }
    return res;
}

LL countCentPaths(int u) {
    LL res = 0;

    vector<ChainInfo> infos;
    for (auto v : G[u]) {
        if (isCent[v]) continue;
        dfsChainSum(v, a[u], a[u], v, 0, infos);
    }
    vector<LL> tmpSumValues;
    prepareNormalization(infos, tmpSumValues);
    res += countInInfos(infos, tmpSumValues, u);
    auto classifyFunc = [](const ChainInfo &a, const ChainInfo &b) {
        return get<2>(a) < get<2>(b);
    };
    sort(infos.begin(), infos.end(), classifyFunc);
    int i = 0;
    while (i < infos.size()) {
        int j = i;
        while (j < infos.size() && get<2>(infos[j]) == get<2>(infos[i])) j++;
        vector<ChainInfo> tmpInfos;
        tmpInfos.reserve(j - i);
        for (int k = i; k < j; k++) {
            tmpInfos.push_back(infos[k]);
        }
        res -= countInInfos(tmpInfos, tmpSumValues, u);
        i = j;
    }
    return res;
}

void dfsCentSolve(int u) {
    // solve problem
    isCent[u] = true;
    ans += countCentPaths(u);
    for (auto v : G[u]) {
        if (isCent[v]) continue;
        dfsCentSolve(findCent(v));
    }
}

int main() {
    ios::sync_with_stdio(false);
    int T;
    cin >> T;
    while (T--) {
        cin >> n;
        for (int i = 1; i <= n; i++) {
            G[i].clear();
            cin >> a[i];
        }
        for (int i = 1; i < n; i++) {
            int u, v;
            cin >> u >> v;
            G[u].push_back(v);
            G[v].push_back(u);
        }

        ans = 0;
        fill(isCent, isCent + MAXN, false);
        dfsCentSolve(findCent(1));
        cout << ans << endl;
    }
    return 0;
}