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

  void initConsole();
  void clearScreen();
  std::string readLinePrompt(const std::string& prompt);
  size_t readSizeT(const std::string& prompt);
  bool readYesNo(const std::string& prompt);
  WordInput readWordInput();
  Crossword createCrosswordInteractive();
  void addWordInteractive(Crossword& crossword);
  void printClues(const Crossword& crossword, std::ostream& out = std::cout);
  void displayCrossword(const Crossword& crossword, std::ostream& out = std::cout);

  void HandleCreate(std::unique_ptr<Crossword>* crossword_ptr);
  void HandleAddWord(Crossword* crossword_ptr);
  void HandleRemoveWord(Crossword* crossword_ptr);
  void HandleDisplay(const Crossword& crossword);
  void HandleValidate(const Crossword& crossword);
  void HandleSave(const Crossword& crossword);
  void HandleLoad(std::unique_ptr<Crossword>* crossword_ptr);
  void HandleSwitchMode(Crossword* crossword_ptr, CrosswordMode new_mode);

  void HandlePlaceAnswer(Crossword* crossword_ptr);
  void HandleRemoveAnswer(Crossword* crossword_ptr);
  void HandleValidateSolution(const Crossword& crossword);
  void HandleSaveSolution(const Crossword& crossword);
  void HandleLoadSolution(std::unique_ptr<Crossword>* crossword_ptr);
}

#endif // CONSOLE_UI_HPP
