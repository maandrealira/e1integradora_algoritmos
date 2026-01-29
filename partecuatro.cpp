#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

static inline bool is_hex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

//huffman tree structures and functions

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

//analysis functions

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

int main() {
    
    ofstream out("partecuatro.txt");
    if (!out) {
        cerr << "No se pudo crear partecuatro.txt\n";
        return 1;
    }
    
    vector<string> tx;
    tx.push_back("txts/transmission1.txt");
    tx.push_back("txts/transmission2.txt");

    vector<string> mcodes;
    mcodes.push_back("txts/mcode1.txt");
    mcodes.push_back("txts/mcode2.txt");
    mcodes.push_back("txts/mcode3.txt");

    for (size_t t = 0; t < tx.size(); ++t) {

        //frecuencias y arbol Huffman por transmision
        auto freq = count_freq(tx[t]);
        HuffNode* root = build_huffman_tree(freq);

        unordered_map<char,int> lens;
        build_codes(root, 0, lens);

        //longitud promedio esperada
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
    return 0;
}


/* 
Para probar este codigo, corra el siguiente comando:
clang++ -std=gnu++14 partecuatro.cpp -o partecuatro 

Luego, ejecute ./partecuatro para gener partecuatro.txt con los resultados
*/




/* Justificación de resultados

Basándonos en la longitud promedio esperada de codificación de cada transmisión, 
no se observa una diferencia significativamente mayor en la longitud codificada 
de los archivos mcode respecto al comportamiento normal del canal. 

Si existiera un caso en el que dicha diferencia fuera anómalamente alta, el código sería marcado como sospechoso. 
Sin embargo, en este conjunto de datos dicha condición no se cumple, esto se debe a que el algoritmo de Huffman no detecta malicia, 
sino rareza estadística, y los mcodes presentan una distribución simbólica consistente con la de las transmisiones, por lo tanto, 
el criterio establecido en el problema no aplica en este caso.
*/