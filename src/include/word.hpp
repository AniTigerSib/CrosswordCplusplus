#ifndef WORD_HPP
#define WORD_HPP

#include "crossword.hpp"
#include <string>
#include <vector>

namespace crossword {
    class Word {
    private:
        Crossword* parent_crossword;
        std::string word;
        std::string clue;
        std::pair<size_t, size_t> startPosition;
        std::vector<std::pair<size_t, size_t>> occupiedCells;
        bool isVertical;
        int number;

    public:
        Word(const std::string& word, const std::string& clue, const std::pair<size_t, size_t>& startPosition, bool isVertical);
        
        const auto& getWord() const { return word; }
        const auto& getClue() const { return clue; }
        const auto& getStartPosition() const { return startPosition; }
        bool getIsVertical() const { return isVertical; }
        int getNumber() const { return number; }
        
        void setStartPosition(const std::pair<size_t, size_t>& newPosition);
        void setIsVertical(bool isVertical);
        void setNumber(int number) { this->number = number; }

        bool isCollidingWith(const Word& other) const;
        bool isValidCollision(const Word& other) const;
    };
}
    
#endif // WORD_HPP