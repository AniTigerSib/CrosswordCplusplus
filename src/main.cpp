#include "include/console_ui.hpp"
#include "include/exceptions.hpp"

#include <iostream>
#include <memory>
#include <string>

namespace {

using crossword::Crossword;
using crossword::CrosswordMode;

void PrintMenu() {
  std::cout << "\n=== Crossword App ===\n"
            << "1. Create crossword\n"
            << "2. Add word\n"
            << "3. Remove word\n"
            << "4. Display crossword\n"
            << "5. Validate crossword\n"
            << "6. Save crossword\n"
            << "7. Load crossword\n"
            << "8. Switch to CREATE mode\n"
            << "9. Switch to SOLVE mode\n"
            << "10. Place answer (SOLVE)\n"
            << "11. Remove answer (SOLVE)\n"
            << "12. Validate solution (SOLVE)\n"
            << "13. Save solution state\n"
            << "14. Load solution state\n"
            << "0. Exit"
            << std::endl;
}

bool EnsureCrosswordLoaded(const std::unique_ptr<Crossword>& crossword_ptr) {
  if (crossword_ptr != nullptr) {
    return true;
  }
  std::cout << "No crossword loaded. Create or load one first." << std::endl;
  return false;
}

}  // namespace

int main() {
  crossword::ui::initConsole();

  std::unique_ptr<Crossword> crossword_ptr;
  bool running = true;

  while (running) {
    try {
      PrintMenu();
      const size_t choice = crossword::ui::readSizeT("Select action: ");

      switch (choice) {
        case 1:
          crossword::ui::HandleCreate(&crossword_ptr);
          break;
        case 2:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            crossword::ui::HandleAddWord(crossword_ptr.get());
          }
          break;
        case 3:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            crossword::ui::HandleRemoveWord(crossword_ptr.get());
          }
          break;
        case 4:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            crossword::ui::HandleDisplay(*crossword_ptr);
          }
          break;
        case 5:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            crossword::ui::HandleValidate(*crossword_ptr);
          }
          break;
        case 6:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            crossword::ui::HandleSave(*crossword_ptr);
          }
          break;
        case 7:
          crossword::ui::HandleLoad(&crossword_ptr);
          break;
        case 8:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            crossword::ui::HandleSwitchMode(crossword_ptr.get(), CrosswordMode::kCreate);
          }
          break;
        case 9:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            crossword::ui::HandleSwitchMode(crossword_ptr.get(), CrosswordMode::kSolve);
          }
          break;
        case 10:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            crossword::ui::HandlePlaceAnswer(crossword_ptr.get());
          }
          break;
        case 11:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            crossword::ui::HandleRemoveAnswer(crossword_ptr.get());
          }
          break;
        case 12:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            crossword::ui::HandleValidateSolution(*crossword_ptr);
          }
          break;
        case 13:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            crossword::ui::HandleSaveSolution(*crossword_ptr);
          }
          break;
        case 14:
          crossword::ui::HandleLoadSolution(&crossword_ptr);
          break;
        case 0:
          running = false;
          break;
        default:
          std::cout << "Unknown menu item." << std::endl;
          break;
      }
    } catch (const crossword::InputInterruptedException&) {
      running = false;
    } catch (const crossword::CrosswordException& e) {
      std::cerr << "Crossword error: " << e.what() << '\n';
    } catch (const std::exception& e) {
      std::cerr << "Unexpected error: " << e.what() << '\n';
    }
  }

  return 0;
}
