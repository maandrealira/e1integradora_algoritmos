#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>

using namespace std;

struct SuffixAutomaton {
    struct State {
        int link = -1;
        int len = 0;
        array<int, 256> next;
        State() { next.fill(-1); }
    };

    vector<State> st;
    int last = 0;

    SuffixAutomaton(int maxLen = 0) {
        st.reserve(2 * maxLen + 1);
        st.push_back(State()); 
        st[0].len = 0;
        st[0].link = -1;
        last = 0;
    }

    void extend(unsigned char c) {
        int cur = (int)st.size();
        st.push_back(State());
        st[cur].len = st[last].len + 1;

        int p = last;
        while (p != -1 && st[p].next[c] == -1) {
            st[p].next[c] = cur;
            p = st[p].link;
        }

        if (p == -1) {
            st[cur].link = 0;
        } else {
            int q = st[p].next[c];
            if (st[p].len + 1 == st[q].len) {
                st[cur].link = q;
            } else {
                int clone = (int)st.size();
                st.push_back(st[q]);
                st[clone].len = st[p].len + 1;

                while (p != -1 && st[p].next[c] == q) {
                    st[p].next[c] = clone;
                    p = st[p].link;
                }

                st[q].link = st[cur].link = clone;
            }
        }
        last = cur;
    }
};

static bool read_file_all(const string& path, string& out) {
    ifstream f(path, ios::binary);
    if (!f) return false;
    out.assign((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const string file1 = "transmission1.txt";
    const string file2 = "transmission2.txt";

    string s1, s2;
    if (!read_file_all(file1, s1) || !read_file_all(file2, s2)) {
        // si no se pueden abrir los archivos:
        cout << "0 0\n";
        return 0;
    }

    
    SuffixAutomaton sam((int)s2.size());
    for (unsigned char c : s2) sam.extend(c);

    
    int v = 0;  // estado actual
    int l = 0;  // longitud actual de coincidencia

    int bestLen = 0;
    int bestEnd = -1; 

    for (int i = 0; i < (int)s1.size(); ++i) {
        unsigned char c = (unsigned char)s1[i];

        while (v != -1 && sam.st[v].next[c] == -1) {
            v = sam.st[v].link;
            if (v != -1) l = sam.st[v].len;
        }

        if (v == -1) {
            v = 0;
            l = 0;
        } else {
            v = sam.st[v].next[c];
            ++l;
        }

        if (l > bestLen) {
            bestLen = l;
            bestEnd = i;
        }
    }

    if (bestLen == 0) {
        cout << "0 0\n";
        return 0;
    }

    int bestStart = bestEnd - bestLen + 1; 
    cout << (bestStart + 1) << " " << (bestEnd + 1) << "\n";
    return 0;
}
