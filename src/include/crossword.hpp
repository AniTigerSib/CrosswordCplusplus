#ifndef CROSSWORD_HPP
#define CROSSWORD_HPP

#include "word.hpp"
#include <functional>
#include <map>
#include <memory>
#include <unordered_map>


namespace std {
    template<>
    struct hash<std::pair<size_t, size_t>> {
        size_t operator()(const std::pair<size_t, size_t>& p) const {
            return hash<size_t>()(p.first) ^ (hash<size_t>()(p.second) << 1);
        }
    };
}

namespace crossword {
    const char kEmptyCell = ' ';
    typedef std::pair<std::vector<std::reference_wrapper<const Word>>, std::vector<std::reference_wrapper<const Word>>> WordsByDirection;

    enum class CrosswordMode {
        kIdle,
        kCreate,
        kSolve
    };

    enum class CellState {
        kEmpty,
        kFilled
    };
    
    class Crossword {
    private:
        std::string title;
        std::pair<size_t, size_t> grid_size;
        std::vector<std::vector<char>> grid;
        std::multimap<int, std::shared_ptr<Word>> numbered_words;
        std::unordered_map<std::string, std::shared_ptr<Word>> word_lookup;
        std::unordered_multimap<std::pair<size_t, size_t>, std::shared_ptr<Word>> word_lookup_by_pos;

        CrosswordMode mode;

        bool isValidWordPlacement(const std::pair<size_t, size_t>& start_position, bool is_vertical, size_t size) const;
        void placeWordsOnGrid();
        void enumerateWords();
    public:
        Crossword(const std::string& title, const std::pair<size_t, size_t>& grid_size) :
            title(title), grid_size(grid_size), mode(CrosswordMode::kIdle) {
                grid.resize(grid_size.first, std::vector<char>(grid_size.second, kEmptyCell));
            }

        const auto& getTitle() const { return title; }
        const auto& getGridSize() const { return grid_size; }
        const WordsByDirection getWords() const;
        const auto& getGrid() const { return grid; }

        void setMode(CrosswordMode new_mode);

        void addWord(const std::string& word, const std::string& clue, const std::pair<size_t, size_t>& start_position, bool is_vertical);
        bool validate() const;
        void removeWord(const std::string& word);
        void removeWordByNumber(int number);
        void clear();

        const Word& findWord(const std::string& word) const;
        std::vector<std::reference_wrapper<const Word>> findWordsByLetter(const char letter) const;
        std::vector<std::reference_wrapper<const Word>> findWordsByLetterAtPosition(const char letter, const size_t pos) const;
        CellState getCellState(const std::pair<size_t, size_t>& position) const;

        void saveToFile(const std::string& filename) const;
        static Crossword loadFromFile(const std::string& filename);
    };
}
    
#endif // CROSSWORD_HPP