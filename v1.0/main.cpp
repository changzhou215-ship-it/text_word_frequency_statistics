#include <iostream> # iostream这个头文件包含了输入输出流的定义，允许我们使用std::cin和std::cout进行输入输出操作。
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: wordTally <file_path>" << std::endl;
        return 1;
    }

    std::string file_path = argv[1];
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << file_path << std::endl;
        return 1;
    }

    std::unordered_map<std::string, int> word_count;
    std::string word;
    int total_words = 0;

    while (file >> word) {
        for (char& c : word) {
            c = std::tolower(static_cast<unsigned char>(c));
        }
        word_count[word]++;
        total_words++;
    }

    file.close();

    std::vector<std::pair<std::string, int>> sorted_words(word_count.begin(), word_count.end());
    std::sort(sorted_words.begin(), sorted_words.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });

    int top_n = std::min(5, static_cast<int>(sorted_words.size()));
    std::cout << "Top " << top_n << " most frequent words:" << std::endl;
    for (int i = 0; i < top_n; i++) {
        std::cout << "'" << sorted_words[i].first << "': " << sorted_words[i].second << std::endl;
    }

    std::cout << "Total words: " << total_words << std::endl;

    return 0;
}
