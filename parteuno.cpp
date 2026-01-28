#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cctype>

using namespace std;

static inline bool is_hex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

string load_pattern_hex_only(const string& path) {
    ifstream in(path);
    if (!in) throw runtime_error("No se pudo abrir: " + path);

    string pat;
    char c;
    while (in.get(c)) {
        if (is_hex(c)) pat.push_back(c);
    }
    return pat;
}

vector<int> build_lps(const string& pat) {
    vector<int> lps(pat.size(), 0);
    int len = 0;
    for (size_t i = 1; i < pat.size();) {
        if (pat[i] == pat[len]) {
            lps[i++] = ++len;
        } else if (len != 0) {
            len = lps[len - 1];
        } else {
            lps[i++] = 0;
        }
    }
    return lps;
}

long long kmp_find_in_file_hex_only(const string& filePath, const string& pat) {
    if (pat.empty()) return 0;

    ifstream in(filePath);
    if (!in) throw runtime_error("No se pudo abrir: " + filePath);

    const int m = (int)pat.size();
    vector<int> lps = build_lps(pat);

    int j = 0;
    long long keptPos = 0;

    char c;
    while (in.get(c)) {
        if (!is_hex(c)) continue;

        while (j > 0 && c != pat[j]) {
            j = lps[j - 1];
        }
        if (c == pat[j]) {
            j++;
            keptPos++;
            if (j == m) {
                return keptPos - m;
            }
        } else {
            keptPos++;
        }
    }
    return -1;
}

int main() {
    try {
        vector<string> codesFiles = {"txts/mcode1.txt", "txts/mcode2.txt", "txts/mcode3.txt"};
        vector<string> txFiles    = {"txts/transmission1.txt", "txts/transmission2.txt"};

        vector<string> patterns;
        patterns.reserve(codesFiles.size());
        for (auto &cf : codesFiles) {
            patterns.push_back(load_pattern_hex_only(cf));
        }

        for (int i = 0; i < (int)codesFiles.size(); i++) {
            for (int t = 0; t < (int)txFiles.size(); t++) {
                long long pos = kmp_find_in_file_hex_only(txFiles[t], patterns[i]);
                if (pos >= 0) cout << "true " << pos << "\n";
                else cout << "false\n";
            }
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
