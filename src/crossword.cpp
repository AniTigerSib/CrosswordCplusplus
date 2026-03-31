#include "include/crossword.hpp"
#include "include/exceptions.hpp"
#include <fstream>

using namespace crossword;

const WordsByDirection Crossword::getWords() const {
    std::vector<std::reference_wrapper<const Word>> horizontal_words;
    std::vector<std::reference_wrapper<const Word>> vertical_words;
    for (const auto& [_, word_ptr] : word_lookup) {
        if (word_ptr->getIsVertical()) {
            vertical_words.push_back(std::cref(*word_ptr));
        } else {
            horizontal_words.push_back(std::cref(*word_ptr));
        }
    }
    return std::move(std::make_pair(horizontal_words, vertical_words));
}

void Crossword::setMode(CrosswordMode new_mode) {
    if (mode == new_mode) {
        return;
    }
    if (mode == CrosswordMode::kCreate) {
        if (!validate()) {
            throw BasicException("Crossword is not valid. Please fix the issues before switching to SOLVE mode.");
        }
    }
    mode = new_mode;
}

void Crossword::addWord(const std::string& word, const std::string& clue, const std::pair<size_t, size_t>& start_position, bool is_vertical) {
    if (mode != CrosswordMode::kCreate) {
        throw BasicException("Cannot add words unless in CREATE mode.");
    }
    if (word.empty() || clue.empty()) {
        throw BasicException("Word and clue cannot be empty.");
    }
    if (word_lookup.find(word) != word_lookup.end()) {
        throw BasicException("Word already exists in the crossword.");
    }
    if (!isValidWordPlacement(start_position, is_vertical, word.length())) {
        throw BasicException("Invalid word placement.");
    }
    for (const auto& [_, existing_word] : word_lookup) {
        auto collision_type = existing_word->isCollidingWith(start_position, word.length(), is_vertical);
        if (collision_type != Word::CollisionType::kNone) {
            if (!existing_word->isValidCollision(start_position, const_cast<std::string&>(word), is_vertical, collision_type)) {
                throw BasicException("Word placement collides with an existing word incorrectly.");
            }
        }
    }
    auto new_word = std::make_shared<Word>(word, clue, start_position, is_vertical);
    word_lookup[word] = new_word;
    word_lookup_by_pos.emplace(start_position, new_word);
    enumerateWords();
    placeWordsOnGrid();
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
        throw BasicException("Cannot remove words unless in CREATE mode.");
    }
    auto it = word_lookup.find(word);
    if (it == word_lookup.end()) {
        throw BasicException("Word does not exist in the crossword.");
    }
    numbered_words.erase(it->second->getNumber());
    auto [pos_it, end_it] = word_lookup_by_pos.equal_range(it->second->getStartPosition());
    if (pos_it == word_lookup_by_pos.end()) {
        throw CriticalException("Critical error: Word position not found in lookup. Please restart the application.");
    } else {
        bool found = false;
        for (; pos_it != end_it && !found; ++pos_it) {
            if (pos_it->second->getWord() == word) {
                found = true;
            }
        }
        if (!found) {
            throw CriticalException("Critical error: Word position not found in lookup. Please restart the application.");
        }
    }
    word_lookup_by_pos.erase(pos_it);
    word_lookup.erase(it);
    enumerateWords();
}

void Crossword::removeWordByNumber(int number) {
    if (mode != CrosswordMode::kCreate) {
        throw BasicException("Cannot remove words unless in CREATE mode.");
    }
    auto it = numbered_words.find(number);
    if (it == numbered_words.end()) {
        throw BasicException("No word exists with the specified number.");
    }
    word_lookup.erase(it->second->getWord());
    auto [pos_it, end_it] = word_lookup_by_pos.equal_range(it->second->getStartPosition());
    if (pos_it == word_lookup_by_pos.end()) {
        throw CriticalException("Critical error: Word position not found in lookup. Please restart the application.");
    } else {
        bool found = false;
        for (; pos_it != end_it && !found; ++pos_it) {
            if (pos_it->second->getNumber() == number) {
                found = true;
            }
        }
        if (!found) {
            throw CriticalException("Critical error: Word position not found in lookup. Please restart the application.");
        }
    }
    word_lookup_by_pos.erase(pos_it);
    numbered_words.erase(it);
    enumerateWords();
}

void Crossword::placeWordsOnGrid() {
    for (auto& row : grid) {
        std::fill(row.begin(), row.end(), kEmptyCell);
    }
    for (const auto& [_, word_ptr] : word_lookup) {
        const auto& word = word_ptr->getWord();
        const auto& start_pos = word_ptr->getStartPosition();
        if (word_ptr->getIsVertical()) {
            for (size_t i = 0; i < word.length(); ++i) {
                grid[start_pos.first + i][start_pos.second] = word[i];
            }
        } else {
            for (size_t i = 0; i < word.length(); ++i) {
                grid[start_pos.first][start_pos.second + i] = word[i];
            }
        }
    }
}

bool Crossword::validate() const {
    // 1) Быстрый выход: пустой кроссворд считаем корректным.
    if (word_lookup.empty()) {
        return true;
    }

    // 2) Собираем слова в индексируемый массив для работы через DSU.
    std::vector<std::shared_ptr<Word>> words;
    words.reserve(word_lookup.size());
    for (const auto& [_, word_ptr] : word_lookup) {
        words.push_back(word_ptr);
    }

    // 3) Инициализация DSU (каждое слово сначала в своем множестве).
    std::vector<size_t> parent(words.size());
    std::vector<size_t> rank(words.size(), 0);
    for (size_t i = 0; i < words.size(); ++i) {
        parent[i] = i;
    }

    // 4) Поиск корня с компрессией пути.
    auto find_root = [&](size_t x) {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    };

    // 5) Объединение множеств по рангу.
    auto unite = [&](size_t a, size_t b) {
        a = find_root(a);
        b = find_root(b);
        if (a == b) {
            return;
        }
        if (rank[a] < rank[b]) {
            parent[a] = b;
        } else if (rank[a] > rank[b]) {
            parent[b] = a;
        } else {
            parent[b] = a;
            ++rank[a];
        }
    };

    // 6) Для каждой занятой клетки храним индекс первого слова-владельца.
    std::unordered_map<std::pair<size_t, size_t>, size_t> cell_owner;
    cell_owner.reserve(words.size() * 6);

    // 7) Проходим по всем буквам всех слов и объединяем слова, если они пересекаются в клетке.
    for (size_t i = 0; i < words.size(); ++i) {
        const auto& word = words[i]->getWord();
        const auto& start = words[i]->getStartPosition();
        const bool vertical = words[i]->getIsVertical();

        for (size_t j = 0; j < word.size(); ++j) {
            const std::pair<size_t, size_t> cell = vertical
                ? std::make_pair(start.first + j, start.second)
                : std::make_pair(start.first, start.second + j);

            auto it = cell_owner.find(cell);
            if (it == cell_owner.end()) {
                cell_owner.emplace(cell, i);
            } else {
                unite(i, it->second);
            }
        }
    }

    // 8) Проверяем, что все слова находятся в одной связной компоненте.
    const size_t first_root = find_root(0);
    for (size_t i = 1; i < words.size(); ++i) {
        if (find_root(i) != first_root) {
            return false;
        }
    }
    return true;
}

void Crossword::enumerateWords() {
    if (mode != CrosswordMode::kCreate) {
        throw BasicException("Cannot enumerate words unless in CREATE mode.");
    }
    if (!numbered_words.empty()) {
        numbered_words.clear();
    }
    int current_number = 1;
    for (int i = 0; i < grid_size.first; ++i) {
        for (int j = 0; j < grid_size.second; ++j) {
            auto [pos_it, end_it] = word_lookup_by_pos.equal_range({i, j});
            if (pos_it == word_lookup_by_pos.end()) {
                continue;
            }
            for (auto it = pos_it; it != end_it; ++it) {
                auto& word = it->second;
                word->setNumber(current_number);
                numbered_words.emplace(current_number, word);
            }
            ++current_number;
        }
    }
}

const Word& Crossword::findWord(const std::string& word) const {
    auto it = word_lookup.find(word);
    if (it != word_lookup.end()) {
        return *it->second;
    }
    throw BasicException("Word not found in the crossword.");
}

std::vector<std::reference_wrapper<const Word>> Crossword::findWordsByLetter(const char letter) const {
    // Легкая обертка над Word по типу ссылки
    std::vector<std::reference_wrapper<const Word>> words;
    for (auto& [word, word_ptr] : word_lookup) {
        if (word.find(letter) != std::string::npos) {
            words.push_back(std::cref(*word_ptr));
        }
    }
    return std::move(words);
}

std::vector<std::reference_wrapper<const Word>> Crossword::findWordsByLetterAtPosition(const char letter, const size_t pos) const {
    std::vector<std::reference_wrapper<const Word>> words;
    for (auto& [word, word_ptr] : word_lookup) {
        if (word[pos] == letter) {
            words.push_back(std::cref(*word_ptr));
        }
    }
    return std::move(words);
}

CellState Crossword::getCellState(const std::pair<size_t, size_t>& position) const {
    if (position.first >= grid_size.first || position.second >= grid_size.second) {
        throw BasicException("Position is out of bounds.");
    }
    return grid[position.first][position.second] == kEmptyCell ? CellState::kEmpty : CellState::kFilled;
}

void Crossword::clear() {
    if (mode != CrosswordMode::kCreate) {
        throw BasicException("Cannot clear crossword unless in CREATE mode.");
    }
    for (auto& row : grid) {
        std::fill(row.begin(), row.end(), kEmptyCell);
    }
    numbered_words.clear();
    word_lookup.clear();
    word_lookup_by_pos.clear();
}

void Crossword::saveToFile(const std::string& filename) const {
    // Реализация сохранения кроссворда в файл. Можно использовать JSON,
    // но для простоты сохраняем в текстовом формате
    std::ofstream out(filename);
    if (!out) {
        throw BasicException("Failed to open file for writing.");
    }
    out << title << "\n";
    out << grid_size.first << " " << grid_size.second << "\n";
    out << word_lookup.size() << "\n";
    for (const auto& [_, word_ptr] : word_lookup) {
        const auto& word = word_ptr->getWord();
        const auto& clue = word_ptr->getClue();
        const auto& start_pos = word_ptr->getStartPosition();
        const bool is_vertical = word_ptr->getIsVertical();
        out << word << "\n" << clue << "\n" << start_pos.first << " " << start_pos.second << " " << is_vertical << "\n";
    }
}

Crossword Crossword::loadFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        throw BasicException("Failed to open file for reading.");
    }
    try {
        std::string title;
        std::getline(in, title);
        size_t rows, cols;
        in >> rows >> cols;
        in.ignore(); // Игнорируем символ новой строки после чисел
        Crossword crossword(title, {rows, cols});
        size_t word_count;
        in >> word_count;
        in.ignore();
        for (size_t i = 0; i < word_count; ++i) {
            std::string word, clue;
            std::getline(in, word);
            std::getline(in, clue);
            size_t start_row, start_col;
            bool is_vertical;
            in >> start_row >> start_col >> is_vertical;
            in.ignore();
            crossword.addWord(word, clue, {start_row, start_col}, is_vertical);
        }
        return crossword;
    } catch (const std::exception& e) {
        throw BasicException("Failed to load crossword from file: " + std::string(e.what()));
    }
}