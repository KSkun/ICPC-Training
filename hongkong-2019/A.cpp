//
// Created by KSkun on 2022/3/28.
//
#include <cassert>
#include <cmath>

#include <iostream>
#include <set>
#include <array>
#include <utility>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

typedef long long LL;

typedef pair<double, double> Point;
typedef pair<Point, Point> Segment;
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
int minX, minY, maxX, maxY;

struct Line {
    double a, b, c;

    Line perpendicularLine(const Point &p) const {
        return {b, -a, a * p.second - b * p.first};
    }

    Point intersect(const Line &another) const {
        auto newX = -(c * another.b - another.c * b) / (a * another.b - another.a * b);
        auto newY = (-c - a * newX) / b;
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

bool checkValidAxis(const Line &axis, const set<PointL> &points) {
    for (auto &p: points) {
        auto symP = axis.symmetricPoint(p);
        // 是否为整点
        bool success = true;
        auto symPL = pointToPointL(symP, success);
        if (!success) return false;
        // 对称点是否存在
        if (points.count(symPL) == 0) return false;
    }
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    int T;
    cin >> T;
    while (T--) {
        cin >> n;
        map<Point, int> overlapCount;
        for (int i = 1; i <= n; i++) {
            int x1, y1, x2, y2;
            cin >> x1 >> y1 >> x2 >> y2;
            if (i == 1) {
                minX = x1;
                minY = y1;
                maxX = x2;
                maxY = y2;
            } else {
                minX = min(minX, x1);
                minY = min(minY, y1);
                maxX = max(maxX, x2);
                maxY = max(maxY, y2);
            }
            overlapCount[{x1, y1}]++;
            overlapCount[{x1, y2}]++;
            overlapCount[{x2, y1}]++;
            overlapCount[{x2, y2}]++;
        }
        // 只比较被一个矩形使用的顶点
        set<PointL> eigenPoints;
        bool tmp;
        for (auto[p, c] : overlapCount) {
            if (c == 1) {
                auto pL = pointToPointL(p, tmp);
                eigenPoints.insert(pL);
            }
        }

        auto bboxMid = midPoint({minX, minY}, {maxX, maxY});
        array<Line, 4> axes = {{
                                       {1, 0, -bboxMid.first},
                                       {0, 1, -bboxMid.second},
                                       {1, 1, -bboxMid.first - bboxMid.second},
                                       {1, -1, -bboxMid.first + bboxMid.second}
                               }};
        vector<LineParams> ans;
        for (auto &a: axes) {
            if (checkValidAxis(a, eigenPoints)) ans.push_back(a.getIntegerParams());
        }
        sort(ans.begin(), ans.end(), greater<LineParams>());
        cout << ans.size() << endl;
        for (auto &p: ans) {
            auto[a, b, c] = p;
            cout << a << " " << b << " " << -c << " ";
        }
        cout << endl;
    }
    return 0;
}