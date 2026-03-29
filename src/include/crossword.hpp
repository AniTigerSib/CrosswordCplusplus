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
        kIdle,
        kCreate,
        kSolve
    };
    
    class Crossword {
    private:
        std::string title;
        std::pair<size_t, size_t> grid_size;
        std::map<int, std::shared_ptr<Word>> numbered_words;
        std::unordered_map<std::string, std::shared_ptr<Word>> word_lookup;
        std::unordered_map<std::pair<size_t, size_t>, std::shared_ptr<Word>> word_lookup_by_pos;
        CrosswordMode mode;

    public:
        Crossword(const std::string& title, const std::pair<size_t, size_t>& grid_size) :
            title(title), grid_size(grid_size), mode(CrosswordMode::kIdle) {}

        const auto& getGridSize() const { return grid_size; }
        const auto& getNumberedWords() const { return numbered_words; }

        void setMode(CrosswordMode new_mode) { mode = new_mode; }

        void addWord(const std::string& word, const std::string& clue, const std::pair<size_t, size_t>& start_position, bool is_vertical);
        bool isValidWordPlacement(const std::pair<size_t, size_t>& start_position, bool is_vertical, size_t size) const;
        void removeWord(const std::string& word);
        
        void enumerateWords();
        void renumberWords();
        // void addUnnumberedWord(const Word& word) { unnumbered_words.push_back(word); }
        // void addNumberedWord(int number, const Word& word) { numbered_words[number] = word; }
    };
}
    
#endif // CROSSWORD_HPP