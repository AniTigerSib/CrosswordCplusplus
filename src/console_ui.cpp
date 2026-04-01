#include "include/console_ui.hpp"
#include "include/exceptions.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>

// Вроде как более корректный вариант работы с неймспейсами
// в фалах исходников
namespace crossword::ui {

void initConsole() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

void clearScreen() {
  // Можно клинить консоль, но для простоты разделим пустыми строками
  std::cout << "\n\n" << std::flush;
}

std::string readLinePrompt(const std::string& prompt) {
  std::cout << prompt << std::flush;
  std::string value;
  if (!std::getline(std::cin, value)) {
    throw InputInterruptedException("Input interrupted.");
  }
  return value;
}

size_t readSizeT(const std::string& prompt) {
  while (true) {
    const std::string token = readLinePrompt(prompt);
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

bool readYesNo(const std::string& prompt) {
  while (true) {
    const std::string token = readLinePrompt(prompt);
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

WordInput readWordInput() {
  WordInput input{};
  input.word = readLinePrompt("Word: ");
  input.clue = readLinePrompt("Clue: ");
  input.row = readSizeT("Start row: ");
  input.col = readSizeT("Start col: ");
  input.is_vertical = readYesNo("Vertical? (y/n): ");
  return input;
}

Crossword createCrosswordInteractive() {
  const std::string title = readLinePrompt("Crossword title: ");
  const size_t rows = readSizeT("Rows: ");
  const size_t cols = readSizeT("Cols: ");

  Crossword crossword(title, {rows, cols});
  crossword.setMode(CrosswordMode::kCreate);
  return crossword;
}

void addWordInteractive(Crossword& crossword) {
  const WordInput input = readWordInput();
  crossword.addWord(input.word, input.clue, {input.row, input.col},
                    input.is_vertical);
}

void printClues(const Crossword& crossword, std::ostream& out) {
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

void displayCrossword(const Crossword& crossword, std::ostream& out) {
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

void HandleCreate(std::unique_ptr<Crossword>* crossword_ptr) {
  *crossword_ptr = std::make_unique<Crossword>(
      crossword::ui::createCrosswordInteractive());
  std::cout << "Crossword created in CREATE mode." << std::endl;
}

void HandleAddWord(Crossword* crossword_ptr) {
  crossword::ui::addWordInteractive(*crossword_ptr);
  std::cout << "Word added." << std::endl;
}

void HandleRemoveWord(Crossword* crossword_ptr) {
  if (crossword::ui::readYesNo("Remove by number? (y/n): ")) {
    const size_t number = crossword::ui::readSizeT("Word number: ");
    const bool is_vertical = crossword::ui::readYesNo("Is vertical? (y/n): ");
    crossword_ptr->removeWordByNumber(static_cast<int>(number), is_vertical);
  } else {
    const std::string word = crossword::ui::readLinePrompt("Word text: ");
    crossword_ptr->removeWord(word);
  }

  std::cout << "Word removed." << std::endl;
}

void HandleDisplay(const Crossword& crossword) {
  crossword::ui::displayCrossword(crossword);
  crossword::ui::printClues(crossword);
}

void HandleValidate(const Crossword& crossword) {
  std::cout << "Crossword is "
            << (crossword.validate() ? "valid." : "invalid.") << std::endl;
}

void HandleSave(const Crossword& crossword) {
  const std::string filename = crossword::ui::readLinePrompt("Save filename: ");
  crossword.saveToFile(filename);
  std::cout << "Saved to '" << filename << "'." << std::endl;
}

void HandleLoad(std::unique_ptr<Crossword>* crossword_ptr) {
  const std::string filename = crossword::ui::readLinePrompt("Load filename: ");
  Crossword loaded = Crossword::loadFromFile(filename);
  *crossword_ptr = std::make_unique<Crossword>(std::move(loaded));
  std::cout << "Crossword loaded in CREATE mode." << std::endl;
}

void HandleSwitchMode(Crossword* crossword_ptr, CrosswordMode mode) {
  crossword_ptr->setMode(mode);
  if (mode == CrosswordMode::kCreate) {
    std::cout << "Switched to CREATE mode." << std::endl;
    return;
  }

  std::cout << "Switched to SOLVE mode." << std::endl;
}

void HandlePlaceAnswer(Crossword* crossword_ptr) {
  const size_t number = crossword::ui::readSizeT("Word number: ");
  const bool is_vertical = crossword::ui::readYesNo("Vertical? (y/n): ");
  const std::string word = crossword::ui::readLinePrompt("Answer: ");
  crossword_ptr->placeAnswer(word, static_cast<int>(number), is_vertical);
  std::cout << "Answer placed." << std::endl;
}

void HandleRemoveAnswer(Crossword* crossword_ptr) {
  const size_t number = crossword::ui::readSizeT("Word number: ");
  const bool is_vertical = crossword::ui::readYesNo("Vertical? (y/n): ");
  crossword_ptr->removeAnswer(static_cast<int>(number), is_vertical);
  std::cout << "Answer removed." << std::endl;
}

void HandleValidateSolution(const Crossword& crossword) {
  const bool is_valid = crossword.validateSolution();
  std::cout << "Solution is " << (is_valid ? "valid." : "invalid.")
            << std::endl;
}

void HandleSaveSolution(const Crossword& crossword) {
  const std::string filename =
      crossword::ui::readLinePrompt("Save solution filename: ");
  crossword.saveSolutionToFile(filename);
  std::cout << "Solution saved to '" << filename << "'." << std::endl;
}

void HandleLoadSolution(std::unique_ptr<Crossword>* crossword_ptr) {
  const std::string filename =
      crossword::ui::readLinePrompt("Load solution filename: ");
  Crossword loaded = Crossword::loadSolutionFromFile(filename);
  *crossword_ptr = std::make_unique<Crossword>(std::move(loaded));
  std::cout << "Solution state loaded from '" << filename << "'."
            << std::endl;
}

}  // namespace crossword::ui
