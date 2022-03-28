//
// Created by KSkun on 2022/3/28.
//
#include <cassert>

#include <iostream>
#include <set>
#include <array>
#include <utility>
#include <vector>
#include <algorithm>

using namespace std;

typedef long long LL;
typedef pair<double, double> Point;

struct Segment {
    Point first, second;

    bool operator<(const Segment &rhs) const {
        return first != rhs.first ? first < rhs.first : second < rhs.second;
    }
};

typedef pair<LL, LL> PointL;
typedef pair<PointL, PointL> SegmentL;

typedef tuple<LL, LL, LL> LineParams;

const double EPS = 1e-6;

int fcmp(double x) {
    if (fabs(x) < EPS) return 0;
    return x < 0 ? -1 : 1;
}

bool samePoint(const Point &a, const Point &b) {
    return fcmp(a.first - b.first) == 0 && fcmp(a.second - b.second) == 0;
}

bool isInteger(double x) {
    return fcmp(x - round(x)) == 0;
}

bool integerPoint(const Point &p) {
    Point rp = {round(p.first), round(p.second)};
    return samePoint(p, rp);
}

Point addPoint(const Point &a, const Point &b) {
    return {a.first + b.first, a.second + b.second};
}

Point minusPoint(const Point &a, const Point &b) {
    return {a.first - b.first, a.second - b.second};
}

Point midPoint(const Point &a, const Point &b) {
    return {(a.first + b.first) / 2, (a.second + b.second) / 2};
}

PointL pointToPointL(const Point &p, bool &success) {
    if (!integerPoint(p)) {
        success = false;
        return {};
    }
    success = true;
    return {(LL) round(p.first), (LL) round(p.second)};
}

SegmentL segmentToSegmentL(const Segment &seg, bool &success) {
    if (!integerPoint(seg.first) || !integerPoint(seg.second)) {
        success = false;
        return {};
    }
    success = true;
    bool tmp;
    return {pointToPointL(seg.first, tmp), pointToPointL(seg.second, tmp)};
}

int n;
Point bboxMin, bboxMax;
vector<Segment *> hSegs, vSegs;

struct Line {
    double a, b, c;

    Line perpendicularLine(const Point &p) const {
        return {b, -a, a * p.second - b * p.first};
    }

    Point intersect(const Line &another) const {
        auto newX = -(c * another.b - another.c * b) / (a * another.b - another.a * b);
        auto newY = -c - a / b * newX;
        return {newX, newY};
    }

    Point symmetricPoint(const Point &p) const {
        auto perp = perpendicularLine(p);
        auto mid = intersect(perp);
        if (fcmp(a) == 0) {
            mid = {p.first, -c / b};
        } else if (fcmp(b) == 0) {
            mid = {-c / a, p.second};
        }
        auto res = addPoint(mid, minusPoint(mid, p));
        return res;
    }

    LineParams getIntegerParams() {
        double na = a, nb = b, nc = c;
        if (!isInteger(na) || !isInteger(nb) || !isInteger(nc)) {
            na *= 2;
            nb *= 2;
            nc *= 2;
        }
        assert(isInteger(na) && isInteger(nb) && isInteger(nc));
        if (na < 0) {
            na *= -1;
            nb *= -1;
            nc *= -1;
        }
        return {round(na), round(nb), round(nc)};
    }
};

void addRectangle(const Point &p1, const Point &p2) {
    auto[x1, y1] = p1;
    auto[x2, y2] = p2;
    hSegs.push_back(new Segment{{x1, y1},
                                {x1, y2}});
    vSegs.push_back(new Segment{{x1, y1},
                                {x2, y1}});
    vSegs.push_back(new Segment{{x1, y2},
                                {x2, y2}});
    hSegs.push_back(new Segment{{x2, y1},
                                {x2, y2}});
}

void eraseDuplicatedSegments(vector<Segment *> &segs) {
    sort(segs.begin(), segs.end());
    for (int i = 0; i < segs.size(); i++) {
        if (i + 1 < segs.size() && segs[i + 1] == segs[i]) {
            segs.erase(segs.begin() + i + 1);
            segs.erase(segs.begin() + i);
            i--;
            continue;
        }
    }
}

vector<Segment> combineHorizonalSegments(vector<Segment *> &segs) {
    auto cmpFunc = [](const Segment *&a, const Segment *&b) {
        if (a->first.first != b->first.first) {
            return a->first.first < b->first.first;
        }
        auto al = a->first.second, ar = a->second.second,
                bl = b->first.second, br = b->second.second;
        return ar != br ? ar < br : al < bl;
    };
    sort(segs.begin(), segs.end(), cmpFunc);
    vector<Segment> newSegs;
    int lIdx = 0;
    while (lIdx < segs.size()) {
        double lineXPos = segs[lIdx]->first.first; // 该行 x 坐标
        int rIdx = lIdx;
        while (rIdx < segs.size() && segs[rIdx]->first.first == segs[lIdx]->first.first) rIdx++;
        // [i, j) 区间内横坐标相同
        double lPos = segs[lIdx]->first.second, rPos = segs[lIdx]->second.second;
        for (int i = lIdx + 1; i < rIdx; i++) {
            double nowLPos = segs[i]->first.second, nowRPos = segs[i]->second.second;
            if (nowLPos > rPos) {
                newSegs.push_back({{lineXPos, lPos},
                                   {lineXPos, rPos}});
                lPos = nowLPos;
                rPos = nowRPos;
                continue;
            }
            rPos = max(nowRPos, rPos);
        }
        newSegs.push_back({{lineXPos, lPos},
                           {lineXPos, rPos}});
        lIdx = rIdx;
    }
    return newSegs;
}

vector<Segment> combineVerticalSegments(vector<Segment *> &segs) {
    auto cmpFunc = [](const Segment *&a, const Segment *&b) {
        if (a->first.second != b->first.second) {
            return a->first.second < b->first.second;
        }
        auto al = a->first.first, ar = a->second.first,
                bl = b->first.first, br = b->second.first;
        return ar != br ? ar < br : al < bl;
    };
    sort(segs.begin(), segs.end(), cmpFunc);
    vector<Segment> newSegs;
    int lIdx = 0;
    while (lIdx < segs.size()) {
        double lineYPos = segs[lIdx]->first.second; // 该行 y 坐标
        int rIdx = lIdx;
        while (rIdx < segs.size() && segs[rIdx]->first.second == segs[lIdx]->first.second) rIdx++;
        // [i, j) 区间内横坐标相同
        double lPos = segs[lIdx]->first.first, rPos = segs[lIdx]->second.first;
        for (int i = lIdx + 1; i < rIdx; i++) {
            double nowLPos = segs[i]->first.first, nowRPos = segs[i]->second.first;
            if (nowLPos > rPos) {
                newSegs.push_back({{lPos, lineYPos},
                                   {rPos, lineYPos}});
                lPos = nowLPos;
                rPos = nowRPos;
                continue;
            }
            rPos = max(nowRPos, rPos);
        }
        newSegs.push_back({{lPos, lineYPos},
                           {rPos, lineYPos}});
        lIdx = rIdx;
    }
    return newSegs;
}

bool checkValidAxis(const Line &axis, const set<SegmentL> &segs) {
    for (auto &seg: segs) {
        Segment symSeg = {axis.symmetricPoint(seg.first), axis.symmetricPoint(seg.second)};
        if (symSeg.first > symSeg.second) swap(symSeg.first, symSeg.second);
        // 是否为整点
        bool success = true;
        auto symSegL = segmentToSegmentL(symSeg, success);
        if (!success) return false;
        // 对称边是否存在
        if (segs.count(symSegL) == 0) return false;
    }
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    int T;
    cin >> T;
    while (T--) {
        cin >> n;
        hSegs.clear();
        vSegs.clear();
        for (int i = 1; i <= n; i++) {
            int x1, y1, x2, y2;
            cin >> x1 >> y1 >> x2 >> y2;
            Point p1 = {x1, y1}, p2 = {x2, y2};
            if (i == 1) {
                bboxMin = p1;
                bboxMax = p2;
            } else {
                bboxMin = min(bboxMin, p1);
                bboxMax = max(bboxMax, p2);
            }
            addRectangle(p1, p2);
        }
        // 删重复边
        eraseDuplicatedSegments(hSegs);
        eraseDuplicatedSegments(vSegs);
        // 重叠边合并
        auto newHSegs = combineHorizonalSegments(hSegs);
        auto newVSegs = combineVerticalSegments(vSegs);
        // 合并边表
        set<SegmentL> segs;
        bool tmp;
        for (auto &seg: hSegs) {
            segs.insert(segmentToSegmentL(*seg, tmp));
        }
        for (auto &seg: vSegs) {
            segs.insert(segmentToSegmentL(*seg, tmp));
        }

        auto bboxMid = midPoint(bboxMin, bboxMax);
        array<Line, 4> axes = {{
                                       {1, 0, -bboxMid.first},
                                       {0, 1, -bboxMid.second},
                                       {1, 1, -bboxMid.first - bboxMid.second},
                                       {1, -1, -bboxMid.first + bboxMid.second}
                               }};
        vector<LineParams> ans;
        for (auto &a: axes) {
            if (checkValidAxis(a, segs)) ans.push_back(a.getIntegerParams());
        }
        sort(ans.begin(), ans.end(), greater());
        cout << ans.size() << endl;
        for (auto &p: ans) {
            auto[a, b, c] = p;
            cout << a << " " << b << " " << c << " ";
        }
        cout << endl;
    }
    return 0;
}