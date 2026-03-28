#include "include/word.hpp"

using namespace crossword;
// TODO: Подумать над оптимизацией коллизий - сейчас это O(n) на каждую проверку, что может быть проблемой при большом количестве слов. Возможно, стоит использовать более сложные структуры данных для отслеживания занятых клеток.
Word::Word(const std::string& word, const std::string& clue, const std::pair<size_t, size_t>& startPosition, bool isVertical) :
    word(word), clue(clue), startPosition(startPosition), isVertical(isVertical), number(0) {
    occupiedCells.reserve(word.length());
    for (size_t i = 0; i < word.length(); ++i) {
        if (isVertical) {
            occupiedCells.emplace_back(startPosition.first + i, startPosition.second);
        } else {
            occupiedCells.emplace_back(startPosition.first, startPosition.second + i);
        }
    }
}

void Word::setStartPosition(const std::pair<size_t, size_t>& newPosition) {
    startPosition = newPosition;
    occupiedCells.clear();
    for (size_t i = 0; i < word.length(); ++i) {
        if (isVertical) {
            occupiedCells.emplace_back(startPosition.first + i, startPosition.second);
        } else {
            occupiedCells.emplace_back(startPosition.first, startPosition.second + i);
        }
    }
}