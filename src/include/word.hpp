#ifndef WORD_HPP
#define WORD_HPP

#include <unordered_set>
#include <string>
#include <vector>

namespace crossword {
  class Word {
  private:
    std::string word;
    std::string clue;
    std::pair<size_t, size_t> start_position;
    bool is_vertical;
    int number;

  public:
    Word(const std::string& word, const std::string& clue, const std::pair<size_t, size_t>& start_position, bool is_vertical)
        : word(word), clue(clue), start_position(start_position), is_vertical(is_vertical), number(0) {}

    const auto& getWord() const { return word; }
    const auto& getClue() const { return clue; }
    const auto& getStartPosition() const { return start_position; }
    const bool getIsVertical() const { return is_vertical; }
    const int getNumber() const { return number; }
        
    void setStartPosition(const std::pair<size_t, size_t>& new_position) { this->start_position = new_position; }
    void setIsVertical(bool is_vertical) { this->is_vertical = is_vertical; }
    void setNumber(int number) { this->number = number; }
    void setWord(const std::string& word) { this->word = word; }
    void setClue(const std::string& clue) { this->clue = clue; }

    enum class CollisionType {
        kNone,
        kSameDirectional,
        kDiffDirectional
    };

    CollisionType isCollidingWith(const std::pair<size_t, size_t>& start_position, size_t size, bool is_vertical) const;
    bool isValidCollision(const std::pair<size_t, size_t>& start_position, const std::string& word, bool is_vertical, CollisionType collision_type) const;
  };

  const bool isBetween(size_t check_pos, size_t basic_pos, size_t size);
}
    
#endif // WORD_HPP