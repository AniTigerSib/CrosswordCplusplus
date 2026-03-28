#include "include/crossword.hpp"

using namespace crossword;

void Crossword::addWord(std::shared_ptr<Word> word) {
    if (mode != CrosswordMode::CREATE) {
        throw std::runtime_error("Cannot add words unless in CREATE mode.");
    }
    if (word_lookup.find(word->getWord()) != word_lookup.end()) {
        throw std::runtime_error("Word already exists in the crossword.");
    }
    if (!isValidWordPlacement(*word)) {
        throw std::runtime_error("Invalid word placement.");
    }
    unnumbered_words.push_back(word);
    word_lookup[word->getWord()] = word;
}

bool Crossword::isValidWordPlacement(const Word& word) const {
    auto [start_row, start_col] = word.getStartPosition();
    auto word_length = word.getWord().length();
    
    // if (word.getIsVertical()) {
    //     if (start_row + word_length > grid_size.first) return false;
    //     for (size_t i = 0; i < word_length; ++i) {
    //         std::string cell_word;
    //         for (const auto& w : unnumbered_words) {
    //             auto [w_row, w_col] = w->getStartPosition();
    //             if (w->getIsVertical() && w_col == start_col && w_row <= start_row + i && w_row + w->getWord().length() > start_row + i) {
    //                 cell_word = w->getWord()[start_row + i - w_row];
    //                 break;
    //             } else if (!w->getIsVertical() && w_row == start_row + i && w_col <= start_col && w_col + w->getWord().length() > start_col) {
    //                 cell_word = w->getWord()[start_col - w_col];
    //                 break;
    //             }
    //         }
    //         if (!cell_word.empty() && cell_word[0] != word.getWord()[i]) {
    //             return false;
    //         }
    //     }
    // } else {
    //     if (start_col + word_length > grid_size.second) return false;
    //     for (size_t i = 0; i < word_length; ++i) {
    //         std::string cell_word;
    //         for (const auto& w : unnumbered_words) {
    //             auto [w_row, w_col] = w->getStartPosition();
    //             if (w->getIsVertical() && w_col == start_col + i && w_row <= start_row && w_row + w->getWord().length() > start_row) {
    //                 cell_word = w->getWord()[start_row - w_row];
    //                 break;
    //             } else if (!w->getIsVertical() && w_row == start_row && w_col <= start_col + i && w_col + w->getWord().length() > start_col + i) {
    //                 cell_word = w->getWord()[start_col + i - w_col];
    //                 break;
    //             }
    //         }
    //         if (!cell_word.empty() && cell_word[0] != word.getWord()[i]) {
    //             return false;
    //         }
    //     }
    // }
    return true;
}