//
// Created by kskun on 2022/4/1.
//

#include <cstdio>
#include <cmath>

#include <array>
#include <queue>
#include <vector>
#include <functional>
#include <algorithm>

using namespace std;

typedef long long LL;

const int MAXN = 2e5 + 5;

int n, m;
LL observVal[MAXN];
int nextID = 0;

struct MemberInfo {
    int k, q[3], alarmCount;
    LL val, baseVal;
    bool done;
} members[MAXN];

struct AlarmInfo {
    int id, alarmCount;
    LL thr;

    bool operator>(const AlarmInfo &rhs) const {
        return thr > rhs.thr;
    }
};

priority_queue<AlarmInfo, vector<AlarmInfo>, greater<AlarmInfo>> alarms[MAXN];

void newAlarms(int id) {
    auto &member = members[id];
    member.alarmCount++;
    int valDiv = ceil(member.val / (double) member.k);
    for (int j = 0; j < member.k; j++) {
        auto &observIdx = member.q[j];
        alarms[observIdx].push({id, member.alarmCount, observVal[observIdx] + valDiv});
    }
}

vector<int> checkAlarms(int idx) {
    vector<int> reachedMembers;
    while (!alarms[idx].empty()) {
        auto info = alarms[idx].top();
        // out-of-date info
        if (info.alarmCount < members[info.id].alarmCount || members[info.id].done) {
            alarms[idx].pop();
            continue;
        }
        // only check reached alarms
        if (info.thr > observVal[idx]) break;

        alarms[idx].pop();
        auto &member = members[info.id];
        LL nowVal = 0;
        for (int j = 0; j < member.k; j++) {
            auto &observIdx = member.q[j];
            nowVal += observVal[observIdx];
        }
        LL nowDiff = nowVal - member.baseVal;
        // already reached goal
        if (nowDiff >= member.val) {
            member.done = true;
            reachedMembers.push_back(info.id);
            continue;
        }
        // left val > 0
        member.val -= nowDiff;
        member.baseVal = nowVal;
        newAlarms(info.id);
    }
    return reachedMembers;
}

int main() {
    scanf("%d%d", &n, &m);
    int last = 0;
    for (int i = 1; i <= m; i++) {
        int type;
        scanf("%d", &type);
        if (type == 1) {
            int id = ++nextID;
            auto &member = members[id];
            scanf("%lld%d", &member.val, &member.k);
            member.val ^= last;
            for (int j = 0; j < member.k; j++) {
                auto &observIdx = member.q[j];
                scanf("%d", &observIdx);
                observIdx ^= last;
                member.baseVal += observVal[observIdx];
            }
            newAlarms(id);
        } else {
            int x, y;
            scanf("%d%d", &x, &y);
            x ^= last;
            y ^= last;

            observVal[x] += y;
            auto ans = checkAlarms(x);
            sort(ans.begin(), ans.end());
            printf("%d", (int) ans.size());
            if (!ans.empty()) printf(" ");
            for (int i = 0; i < ans.size(); i++) {
                printf("%d", ans[i]);
                if (i != ans.size() - 1) printf(" ");
            }
            printf("\n");
            last = ans.size();
        }
    }
    return 0;
}