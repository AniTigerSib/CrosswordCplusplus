#include "include/console_ui.hpp"
#include "include/exceptions.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>

namespace crossword::ui {

void InitConsole() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

void ClearScreen() {
  // Plain console output: separate screens with blank lines.
  std::cout << "\n\n" << std::flush;
}

std::string ReadLinePrompt(const std::string& prompt) {
  std::cout << prompt << std::flush;
  std::string value;
  if (!std::getline(std::cin, value)) {
    throw InputInterruptedException("Input interrupted.");
  }
  return value;
}

size_t ReadSizeT(const std::string& prompt) {
  while (true) {
    const std::string token = ReadLinePrompt(prompt);
    if (token.empty()) {
      std::cout << "Invalid input. Try again." << std::endl;
      continue;
    }

    try {
      size_t parsed = 0;
      const size_t value = std::stoull(token, &parsed);
      if (parsed == token.size()) {
        return value;
      }
    } catch (const std::exception&) {
      // Fall through to retry message.
    }

    std::cout << "Invalid input. Try again." << std::endl;
  }
}

bool ReadYesNo(const std::string& prompt) {
  while (true) {
    const std::string token = ReadLinePrompt(prompt);
    if (token.empty()) {
      continue;
    }

    const char c = static_cast<char>(
        std::tolower(static_cast<unsigned char>(token.front())));
    if (c == 'y' || c == '1') {
      return true;
    }
    if (c == 'n' || c == '0') {
      return false;
    }

    std::cout << "Enter y/n." << std::endl;
  }
}

WordInput ReadWordInput() {
  WordInput input{};
  input.word = ReadLinePrompt("Word: ");
  input.clue = ReadLinePrompt("Clue: ");
  input.row = ReadSizeT("Start row: ");
  input.col = ReadSizeT("Start col: ");
  input.is_vertical = ReadYesNo("Vertical? (y/n): ");
  return input;
}

Crossword CreateCrosswordInteractive() {
  const std::string title = ReadLinePrompt("Crossword title: ");
  const size_t rows = ReadSizeT("Rows: ");
  const size_t cols = ReadSizeT("Cols: ");

  Crossword crossword(title, {rows, cols});
  crossword.setMode(CrosswordMode::kCreate);
  return crossword;
}

void AddWordInteractive(Crossword& crossword) {
  const WordInput input = ReadWordInput();
  crossword.addWord(input.word, input.clue, {input.row, input.col},
                    input.is_vertical);
}

void PrintClues(const Crossword& crossword, std::ostream& out) {
  auto [across_words, down_words] = crossword.getWords();
  const auto by_number =
      [](const std::reference_wrapper<const Word>& lhs,
         const std::reference_wrapper<const Word>& rhs) {
        return lhs.get().getNumber() < rhs.get().getNumber();
      };

  std::sort(across_words.begin(), across_words.end(), by_number);
  std::sort(down_words.begin(), down_words.end(), by_number);

  out << "\nAcross:\n";
  for (const auto& word_ref : across_words) {
    const Word& word = word_ref.get();
    out << word.getNumber() << ". " << word.getClue() << '\n';
  }

  out << "\nDown:\n";
  for (const auto& word_ref : down_words) {
    const Word& word = word_ref.get();
    out << word.getNumber() << ". " << word.getClue() << '\n';
  }
}

void DisplayCrossword(const Crossword& crossword, std::ostream& out) {
  const auto& grid = crossword.getGrid();
  const auto [rows, cols] = crossword.getGridSize();

  out << "Title: " << crossword.getTitle() << '\n';
  out << "Size: " << rows << "x" << cols << "\n\n";

  for (size_t row = 0; row < rows; ++row) {
    for (size_t col = 0; col < cols; ++col) {
      const char ch = grid[row][col];
      out << (ch == kEmptyCell ? '.' : ch) << ' ';
    }
    out << '\n';
  }
}

void SaveCrosswordToFile(const Crossword& crossword, const std::string& filename) {
  crossword.saveToFile(filename);
}

Crossword LoadCrosswordFromFile(const std::string& filename) {
  return Crossword::loadFromFile(filename);
}

}  // namespace crossword::ui
