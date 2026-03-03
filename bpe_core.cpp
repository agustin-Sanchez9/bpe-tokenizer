#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <regex>

// Usamos unordered_map para busqueda en tiempo constante O(1)
// Necesitamos un hash para el pair de ints
struct pair_hash {
    inline std::size_t operator()(const std::pair<int, int> & v) const {
        return v.first * 31 + v.second;
    }
};

typedef std::pair<int, int> Pair;
typedef std::unordered_map<Pair, int, pair_hash> Stats;

void get_stats(const std::vector<int>& ids, Stats& stats) {
    stats.clear();
    for (size_t i = 0; i < ids.size() - 1; ++i) {
        stats[{ids[i], ids[i+1]}]++;
    }
}

// Merge "in-place" para evitar copias masivas de memoria
void perform_merge(std::vector<int>& ids, Pair pair, int replacement_id) {
    size_t write_idx = 0;
    for (size_t read_idx = 0; read_idx < ids.size(); ++read_idx) {
        if (read_idx < ids.size() - 1 && ids[read_idx] == pair.first && ids[read_idx+1] == pair.second) {
            ids[write_idx++] = replacement_id;
            read_idx++;
        } else {
            ids[write_idx++] = ids[read_idx];
        }
    }
    ids.resize(write_idx); // Ajustar el tamaño final sin reasignar memoria
}

void perform_merge_in_section(std::vector<int>& section, Pair pair, int replacement_id) {
    if (section.size() < 2) return;
    size_t write_idx = 0;
    for (size_t read_idx = 0; read_idx < section.size(); ++read_idx) {
        if (read_idx < section.size() - 1 && section[read_idx] == pair.first && section[read_idx+1] == pair.second) {
            section[write_idx++] = replacement_id;
            read_idx++;
        } else {
            section[write_idx++] = section[read_idx];
        }
    }
    section.resize(write_idx);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: bpe_core <archivo_corpus> <num_merges>" << std::endl;
        return 1;
    }

    // Lectura del archivo corpus.bin
    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Error: No se pudo abrir el archivo." << std::endl;
        return 1;
    }
    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string text(buffer.begin(), buffer.end());
    file.close();

    // Este regex emula el utilizado en gpt-2 " ?\p{L}+| ?\p{N}+| ?[^\s\p{L}\p{N}]+|\s+(?!\S)|\s+" 
    std::regex bpe_pattern(R"( ?[a-zA-ZáéíóúÁÉÍÓÚñÑ]+| ?[0-9]+| ?[^\s\w]+|\s+(?!\S)|\s+)", std::regex::optimize);
    
    auto words_begin = std::sregex_iterator(text.begin(), text.end(), bpe_pattern);
    auto words_end = std::sregex_iterator();

    std::vector<std::vector<int>> split_ids;
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::string match_str = i->str();
        std::vector<int> section;
        for (unsigned char c : match_str) {
            section.push_back(static_cast<int>(c));
        }
        split_ids.push_back(section);
    }

    int num_merges = std::stoi(argv[2]);
    std::ofstream merges_file("merges.txt");

    // Bucle de creacion de reglas
    for (int i = 0; i < num_merges; ++i) {
        Stats stats;
        
        // Contar pares SOLO dentro de cada seccion
        for (const auto& section : split_ids) {
            if (section.size() < 2) continue;
            for (size_t j = 0; j < section.size() - 1; ++j) {
                stats[{section[j], section[j+1]}]++;
            }
        }

        if (stats.empty()) break;

        // Encontrar el par mas frecuente
        Pair best_pair;
        int max_freq = -1;
        for (auto const& it : stats) {
            if (it.second > max_freq) {
                max_freq = it.second;
                best_pair = it.first;
            }
        }

        if (max_freq <= 0) break;

        // Guardar regla
        merges_file << best_pair.first << " " << best_pair.second << "\n";

        // Aplicar la fusion en cada seccion
        size_t total_size = 0;
        for (auto& section : split_ids) {
            perform_merge_in_section(section, best_pair, 256 + i);
            total_size += section.size();
        }

        if ((i + 1) % 100 == 0) {
            std::cout << "Fusion " << (i + 1) << " | Par: (" << best_pair.first 
                      << "," << best_pair.second << ") | Tamanio: " << total_size << std::endl;
        }
    }

    merges_file.close();
    std::cout << "Entrenamiento finalizado." << std::endl;
    return 0;
}