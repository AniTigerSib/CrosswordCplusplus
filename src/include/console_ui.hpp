#ifndef CONSOLE_UI_HPP
#define CONSOLE_UI_HPP

#include "crossword.hpp"
#include <string>
#include <ostream>
#include <iostream>

namespace crossword::ui {
  struct WordInput {
    std::string word;
    std::string clue;
    size_t row;
    size_t col;
    bool is_vertical;
  };

  void InitConsole();
  void ClearScreen();
  std::string ReadLinePrompt(const std::string& prompt);
  size_t ReadSizeT(const std::string& prompt);
  bool ReadYesNo(const std::string& prompt);
  WordInput ReadWordInput();
  Crossword CreateCrosswordInteractive();
  void AddWordInteractive(Crossword& crossword);
  void PrintClues(const Crossword& crossword, std::ostream& out = std::cout);
  void DisplayCrossword(const Crossword& crossword, std::ostream& out = std::cout);
  void SaveCrosswordToFile(const Crossword& crossword, const std::string& filename);
  Crossword LoadCrosswordFromFile(const std::string& filename);
}

#endif // CONSOLE_UI_HPP
