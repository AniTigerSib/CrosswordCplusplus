#include "include/crossword.hpp"

using namespace crossword;

void Crossword::addWord(const std::string& word, const std::string& clue, const std::pair<size_t, size_t>& start_position, bool is_vertical) {
    if (mode != CrosswordMode::kCreate) {
        throw std::runtime_error("Cannot add words unless in CREATE mode.");
    }
    if (word_lookup.find(word) != word_lookup.end()) {
        throw std::runtime_error("Word already exists in the crossword.");
    }
    if (!isValidWordPlacement(start_position, is_vertical, word.length())) {
        throw std::runtime_error("Invalid word placement.");
    }
    for (const auto& [_, existing_word] : word_lookup) {
        auto collision_type = existing_word->isCollidingWith(start_position, word.length(), is_vertical);
        if (collision_type != Word::CollisionType::kNone) {
            if (!existing_word->isValidCollision(start_position, const_cast<std::string&>(word), is_vertical, collision_type)) {
                throw std::runtime_error("Word placement collides with an existing word incorrectly.");
            }
        }
    }
    auto new_word = std::make_shared<Word>(word, clue, start_position, is_vertical);
    word_lookup[word] = new_word;
    word_lookup_by_pos[start_position] = new_word;
}

bool Crossword::isValidWordPlacement(const std::pair<size_t, size_t>& start_position, bool is_vertical, size_t size) const {
    if (is_vertical) {
        if (start_position.second > this->grid_size.second ||
            start_position.first + size > this->grid_size.first)
            return false;
    } else {
        if (start_position.first > this->grid_size.first ||
            start_position.second + size > this->grid_size.second)
            return false;
    }
    return true;
}

void Crossword::removeWord(const std::string& word) {
    if (mode != CrosswordMode::kCreate) {
        throw std::runtime_error("Cannot remove words unless in CREATE mode.");
    }
    auto it = word_lookup.find(word);
    if (it == word_lookup.end()) {
        throw std::runtime_error("Word does not exist in the crossword.");
    }
    word_lookup_by_pos.erase(it->second->getStartPosition());
    word_lookup.erase(it);
}

