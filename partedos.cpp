#include <iostream>
#include <string>
#include <utility>

using namespace std;

static pair<int,int> expand_center(const string& s, int L, int R) {
    int n = (int)s.size();
    while (L >= 0 && R < n && s[L] == s[R]) {
        --L;
        ++R;
    }
    return {L + 1, R - 1};
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string s, line;
    bool first = true;

    while (getline(cin, line)) {
        if (!first) s.push_back('\n');
        first = false;
        s += line;
    }

    int n = (int)s.size();
    if (n == 0) return 0;

    int bestL = 0, bestR = 0, bestLen = 1;

    for (int i = 0; i < n; ++i) {
        auto [l1, r1] = expand_center(s, i, i);
        int len1 = r1 - l1 + 1;
        if (len1 > bestLen) {
            bestLen = len1;
            bestL = l1;
            bestR = r1;
        }

        if (i + 1 < n) {
            auto [l2, r2] = expand_center(s, i, i + 1);
            int len2 = r2 - l2 + 1;
            if (len2 > bestLen) {
                bestLen = len2;
                bestL = l2;
                bestR = r2;
            }
        }
    }

    cout << (bestL + 1) << " " << (bestR + 1) << "\n";
    return 0;
}

//para ejecutar el código, corre en terminal:
// g++ -o partedos partedos.cpp
// ./partedos < input.txt