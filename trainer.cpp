#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace py = pybind11;

// estructura para usar pares como llaves en el hash map
struct Pair {
    int first, second;
    bool operator==(const Pair& other) const {
        return first == other.first && second == other.second;
    }
};

struct PairHash {
    size_t operator()(const Pair& p) const {
        return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
    }
};

// funcion interna para contar pares
std::unordered_map<Pair, int, PairHash> get_stats(const std::vector<int>& ids) {
    std::unordered_map<Pair, int, PairHash> counts;
    if (ids.size() < 2) return counts;
    for (size_t i = 0; i < ids.size() - 1; ++i) {
        counts[{ids[i], ids[i+1]}]++;
    }
    return counts;
}

// funcion interna para fusionar
std::vector<int> perform_merge(const std::vector<int>& ids, Pair pair, int replacement_id) {
    std::vector<int> new_ids;
    new_ids.reserve(ids.size()); // reserva de memoria
    for (size_t i = 0; i < ids.size(); ++i) {
        if (i < ids.size() - 1 && ids[i] == pair.first && ids[i+1] == pair.second) {
            new_ids.push_back(replacement_id);
            i++;
        } else {
            new_ids.push_back(ids[i]);
        }
    }
    return new_ids;
}

// funcion principal a llamar en python
std::vector<std::pair<int, int>> train_bpe_cpp(std::vector<uint8_t> raw_bytes, int num_merges) {
    std::vector<int> ids(raw_bytes.begin(), raw_bytes.end());
    std::vector<std::pair<int, int>> merges;

    for (int i = 0; i < num_merges; ++i) {
        auto stats = get_stats(ids);
        if (stats.empty()) break;

        // par mas frecuente
        auto most_frequent = std::max_element(stats.begin(), stats.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });

        Pair best_pair = most_frequent->first;
        int new_token_id = 256 + i;

        ids = perform_merge(ids, best_pair, new_token_id);
        merges.push_back({best_pair.first, best_pair.second});
    }

    return merges;
}

// definicion del modulo para Python
PYBIND11_MODULE(bpe_core, m) {
    m.def("train_bpe", &train_bpe_cpp, "Entrena BPE de forma acelerada");
}