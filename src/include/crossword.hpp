#ifndef CROSSWORD_HPP
#define CROSSWORD_HPP

#include "word.hpp"
#include <map>
#include <unordered_map>
#include <memory>

namespace std {
    template<>
    struct hash<std::pair<size_t, size_t>> {
        size_t operator()(const std::pair<size_t, size_t>& p) const {
            return hash<size_t>()(p.first) ^ (hash<size_t>()(p.second) << 1);
        }
    };
}

namespace crossword {
    enum class CrosswordMode {
        IDLE,
        CREATE,
        SOLVE
    };
    
    class Crossword {
    private:
        std::string title;
        std::pair<size_t, size_t> grid_size;
        std::vector<std::shared_ptr<Word>> unnumbered_words;
        std::map<int, std::shared_ptr<Word>> numbered_words;
        std::map<std::pair<size_t, size_t>, std::shared_ptr<Word>> vertical_word_lookup;
        std::map<std::pair<size_t, size_t>, std::shared_ptr<Word>> horizontal_word_lookup;
        std::unordered_map<std::string, std::shared_ptr<Word>> word_lookup;
        CrosswordMode mode;

    public:
        Crossword(const std::string& title, const std::pair<size_t, size_t>& grid_size) :
            title(title), grid_size(grid_size), mode(CrosswordMode::IDLE) {}
        
        const auto& getGridSize() const { return grid_size; }
        const auto& getUnnumberedWords() const { return unnumbered_words; }
        const auto& getNumberedWords() const { return numbered_words; }
        
        void setMode(CrosswordMode new_mode) { mode = new_mode; }

        void addWord(std::shared_ptr<Word> word);
        bool isValidWordPlacement(const Word& word) const;
        void removeWord(const std::string& word);
        
        void enumerateWords();
        void renumberWords();
        // void addUnnumberedWord(const Word& word) { unnumbered_words.push_back(word); }
        // void addNumberedWord(int number, const Word& word) { numbered_words[number] = word; }
    };
}
    
#endif // CROSSWORD_HPP