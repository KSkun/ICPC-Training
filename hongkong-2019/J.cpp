//
// Created by kskun on 2022/4/1.
//

#include <cstdio>
#include <cstring>

#include <algorithm>

using namespace std;

typedef long long LL;

const int MO = 1e9 + 7;

char LStr[5005], RStr[5005];
int limitDigits[5005], limitLen, m, f[5005][61][61][2];

void prepareLimitDigits(char numStr[], bool isL) {
    limitLen = strlen(numStr);
    for (int i = 0; i < limitLen; i++) {
        limitDigits[limitLen - i - 1] = numStr[i] - '0';
    }
    // minus 1 before convert to binary
    if (isL) {
        limitDigits[0]--;
        for (int i = 0; i < limitLen; i++) {
            if (limitDigits[i] < 0) {
                limitDigits[i] += 10;
                limitDigits[i + 1]--;
            } else break;
        }
        for (int i = limitLen - 1; i >= 0; i--) {
            if (limitDigits[i] == 0) limitLen--;
            else break;
        }
    }
    reverse(limitDigits, limitDigits + limitLen);
}

int pow10[5005];

void workDP() {
    for (int step = 0; step < limitLen; step++) {
        for (int fxMinusX = 0; fxMinusX < m; fxMinusX++) {
            for (int digitSum = 0; digitSum < m; digitSum++) {
                // no limit
                int &fVal0 = f[step][fxMinusX][digitSum][0];
                if (fVal0 > 0) {
                    for (int i = 0; i <= 9; i++) {
                        int newFx = ((fxMinusX + i * digitSum - i * pow10[limitLen - step - 1]) % m + m) % m;
                        int newDigitSum = (digitSum + i) % m;
                        int &subFVal = f[step + 1][newFx][newDigitSum][0];
                        subFVal = (subFVal + fVal0) % MO;
                    }
                }

                // under limit
                int &fVal1 = f[step][fxMinusX][digitSum][1];
                if (fVal1 > 0) {
                    for (int i = 0; i <= limitDigits[step]; i++) {
                        int newFx = ((fxMinusX + i * digitSum - i * pow10[limitLen - step - 1]) % m + m) % m;
                        int newDigitSum = (digitSum + i) % m;
                        int &subFVal = f[step + 1][newFx][newDigitSum][limitDigits[step] == i];
                        subFVal = (subFVal + fVal1) % MO;
                    }
                }
            }
        }
    }
}

int main() {
    int T;
    scanf("%d", &T);
    while (T--) {
        scanf("%s%s%d", LStr, RStr, &m);
        pow10[0] = 1;
        int len = max(strlen(LStr), strlen(RStr));
        for (int i = 1; i <= len; i++) {
            pow10[i] = (pow10[i - 1] * 10) % m;
        }

        f[0][0][0][1] = 1;
        prepareLimitDigits(LStr, true);
        workDP();
        LL LAns = 0;
        for (int i = 0; i < m; i++) {
            LAns += f[limitLen][0][i][0];
            LAns += f[limitLen][0][i][1];
            if (LAns > 1e15) LAns %= MO;
        }
        for (int i = 0; i <= limitLen; i++) {
            memset(f[i], 0, sizeof(f[i]));
        }


        f[0][0][0][1] = 1;
        prepareLimitDigits(RStr, false);
        workDP();
        LL RAns = 0;
        for (int i = 0; i < m; i++) {
            RAns += f[limitLen][0][i][0];
            RAns += f[limitLen][0][i][1];
            if (RAns > 1e15) RAns %= MO;
        }
        for (int i = 0; i <= limitLen; i++) {
            memset(f[i], 0, sizeof(f[i]));
        }

        LL ans = ((RAns - LAns) % MO + MO) % MO;
        printf("%lld\n", ans);
    }
    return 0;
}