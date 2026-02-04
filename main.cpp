#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cctype>
#include <utility>
#include <array>
#include <queue>
#include <unordered_map>

using namespace std;

//Part 1: KMP Search in Hex Files
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

void parte_uno() {
    cout << "\n=== Part 1: KMP Search en Archivos Hex ===\n";
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
        cerr << "Error en Parte 1: " << e.what() << "\n";
    }
}

//Part 2: Longest Palindromic Substring

static pair<int,int> expand_center(const string& s, int L, int R) {
    int n = (int)s.size();
    while (L >= 0 && R < n && s[L] == s[R]) {
        --L;
        ++R;
    }
    return {L + 1, R - 1};
}

void parte_dos(const string& input_text) {
    cout << "\n==Part 2: Longest Palindromic Substring ===\n";
    
    string s = input_text;
    int n = (int)s.size();
    if (n == 0) return;
    
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
}

//Part 3: Suffix Automaton for Longest Common Substring

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

void parte_tres() {
    cout << "\n=== Part 3: Suffix Automaton ===\n";
    
    const string file1 = "txts/transmission1.txt";
    const string file2 = "txts/transmission2.txt";
    
    string s1, s2;
    if (!read_file_all(file1, s1) || !read_file_all(file2, s2)) {
        cout << "0 0\n";
        return;
    }
    
    SuffixAutomaton sam((int)s2.size());
    for (unsigned char c : s2) sam.extend(c);
    
    int v = 0;
    int l = 0;
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
        return;
    }
    
    int bestStart = bestEnd - bestLen + 1;
    cout << (bestStart + 1) << " " << (bestEnd + 1) << "\n";
}

// Part 4: Huffman Coding Analysis

struct HuffNode {
    char ch;
    long long freq;
    HuffNode *left, *right;
    
    HuffNode(char c, long long f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    HuffNode(HuffNode* l, HuffNode* r)
        : ch(0), freq(l->freq + r->freq), left(l), right(r) {}
};

struct Cmp {
    bool operator()(HuffNode* a, HuffNode* b) const {
        return a->freq > b->freq;
    }
};

void build_codes(HuffNode* node, int depth,
                 unordered_map<char,int>& lengths) {
    if (!node) return;
    if (!node->left && !node->right) {
        lengths[node->ch] = depth;
        return;
    }
    build_codes(node->left, depth + 1, lengths);
    build_codes(node->right, depth + 1, lengths);
}

HuffNode* build_huffman_tree(const unordered_map<char,long long>& freq) {
    priority_queue<HuffNode*, vector<HuffNode*>, Cmp> pq;
    
    for (auto it = freq.begin(); it != freq.end(); ++it) {
        pq.push(new HuffNode(it->first, it->second));
    }
    
    while (pq.size() > 1) {
        HuffNode* a = pq.top(); pq.pop();
        HuffNode* b = pq.top(); pq.pop();
        pq.push(new HuffNode(a, b));
    }
    return pq.top();
}

unordered_map<char,long long> count_freq(const string& path) {
    ifstream in(path.c_str());
    if (!in) throw runtime_error("No se pudo abrir: " + path);
    
    unordered_map<char,long long> freq;
    char c;
    while (in.get(c)) {
        if (is_hex(c)) freq[c]++;
    }
    return freq;
}

long long encoded_length(const string& path,
                         const unordered_map<char,int>& lens,
                         long long& symbols) {
    ifstream in(path.c_str());
    if (!in) throw runtime_error("No se pudo abrir: " + path);
    
    long long bits = 0;
    symbols = 0;
    char c;
    while (in.get(c)) {
        if (is_hex(c)) {
            bits += lens.at(c);
            symbols++;
        }
    }
    return bits;
}

double expected_avg_length(const unordered_map<char,long long>& freq,
                           const unordered_map<char,int>& lens) {
    double total = 0.0, acc = 0.0;
    
    for (auto it = freq.begin(); it != freq.end(); ++it)
        total += it->second;
    
    for (auto it = freq.begin(); it != freq.end(); ++it)
        acc += (it->second / total) * lens.at(it->first);
    
    return acc;
}

void parte_cuatro() {
    cout << "\n=== Part 4: Huffman Coding Analysis ===\n";
    
    ofstream out("partecuatro.txt");
    if (!out) {
        cerr << "No se pudo crear partecuatro.txt\n";
        return;
    }
    
    vector<string> tx;
    tx.push_back("txts/transmission1.txt");
    tx.push_back("txts/transmission2.txt");
    
    vector<string> mcodes;
    mcodes.push_back("txts/mcode1.txt");
    mcodes.push_back("txts/mcode2.txt");
    mcodes.push_back("txts/mcode3.txt");
    
    for (size_t t = 0; t < tx.size(); ++t) {
        auto freq = count_freq(tx[t]);
        HuffNode* root = build_huffman_tree(freq);
        
        unordered_map<char,int> lens;
        build_codes(root, 0, lens);
        
        double Lavg = expected_avg_length(freq, lens);
        
        long long tx_symbols = 0;
        long long tx_bits = encoded_length(tx[t], lens, tx_symbols);
        
        for (size_t i = 0; i < mcodes.size(); ++i) {
            long long symbols = 0;
            long long bits = encoded_length(mcodes[i], lens, symbols);
            double Lm = (symbols > 0) ? (double)bits / symbols : 0.0;
            
            if (Lm > 1.5 * Lavg)
                out << "sospechoso " << bits << "\n";
            else
                out << "no-sospechoso " << bits << "\n";
        }
    }
    cout << "Resultados guardados en partecuatro.txt\n";
}

// ============================================================================
// MAIN
// ============================================================================

int main() {
    cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
    cout << "║          Ejecutando todas las partes en orden            ║\n";
    cout << "╚═══════════════════════════════════════════════════════════╝\n";
    
    parte_uno();
    
    // Para parte 2, leer entrada estándar
    cout << "\nParte 2 requiere entrada (paste el texto o ctrl+D para terminar):\n";
    string input_line;
    string full_input;
    bool first = true;
    while (getline(cin, input_line)) {
        if (!first) full_input.push_back('\n');
        first = false;
        full_input += input_line;
    }
    parte_dos(full_input);
    
    parte_tres();
    parte_cuatro();
    
    return 0;
}
