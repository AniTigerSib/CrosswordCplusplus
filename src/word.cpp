#include "include/word.hpp"

using namespace crossword;

const bool crossword::isBetween(size_t check_pos, size_t basic_pos, size_t size) {
    if (check_pos >= basic_pos && check_pos <= basic_pos + size)
        return true;
    return false;
}

Word::CollisionType Word::isCollidingWith(const std::pair<size_t, size_t>& start_position, size_t size, bool is_vertical) const {
    if (this->is_vertical == true) {
        if (is_vertical == true) {
            if (this->start_position.second == start_position.second &&
                (isBetween(this->start_position.first, start_position.first, size) ||
                 isBetween(start_position.first, this->start_position.first, this->word.size())))
                return CollisionType::kSameDirectional;
        } else {
            if (isBetween(this->start_position.second, start_position.second, size) &&
                isBetween(start_position.first, this->start_position.first, this->word.size()))
                return CollisionType::kDiffDirectional;
        }
    } else {
        if (is_vertical == true) {
            if (isBetween(start_position.second, this->start_position.second, this->word.size()) &&
                isBetween(this->start_position.first, start_position.first, size))
                return CollisionType::kDiffDirectional;
        } else {
            if (this->start_position.first == start_position.first &&
                (isBetween(this->start_position.second, start_position.second, size) ||
                 isBetween(start_position.second, this->start_position.second, this->word.size())))
                return CollisionType::kSameDirectional;
        }
    }
    return CollisionType::kNone;
}

bool Word::isValidCollision(const std::pair<size_t, size_t>& start_position, const std::string& word, bool is_vertical, CollisionType collision_type) const {
    if (collision_type == CollisionType::kNone)
        return true;
    if (collision_type == CollisionType::kSameDirectional)
        return false;
    char letter1 = '\0';
    char letter2 = '\0';
    if (this->is_vertical) {
        letter1 = this->word[start_position.first - this->start_position.first];
        letter2 = word[this->start_position.second - start_position.second];
    } else {
        letter1 = this->word[this->start_position.first - start_position.first];
        letter2 = word[start_position.second - this->start_position.second];
    }
    return letter1 == letter2;
}