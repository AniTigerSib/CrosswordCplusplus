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

void HandleCreate(std::unique_ptr<Crossword>* crossword_ptr) {
  *crossword_ptr = std::make_unique<Crossword>(
      crossword::ui::CreateCrosswordInteractive());
  std::cout << "Crossword created in CREATE mode." << std::endl;
}

void HandleAddWord(Crossword* crossword_ptr) {
  crossword::ui::AddWordInteractive(*crossword_ptr);
  std::cout << "Word added." << std::endl;
}

void HandleRemoveWord(Crossword* crossword_ptr) {
  if (crossword::ui::ReadYesNo("Remove by number? (y/n): ")) {
    const size_t number = crossword::ui::ReadSizeT("Word number: ");
    crossword_ptr->removeWordByNumber(static_cast<int>(number));
  } else {
    const std::string word = crossword::ui::ReadLinePrompt("Word text: ");
    crossword_ptr->removeWord(word);
  }

  std::cout << "Word removed." << std::endl;
}

void HandleDisplay(const Crossword& crossword) {
  crossword::ui::DisplayCrossword(crossword);
  crossword::ui::PrintClues(crossword);
}

void HandleValidate(const Crossword& crossword) {
  std::cout << "Crossword is "
            << (crossword.validate() ? "valid." : "invalid.") << std::endl;
}

void HandleSave(const Crossword& crossword) {
  const std::string filename = crossword::ui::ReadLinePrompt("Save filename: ");
  crossword::ui::SaveCrosswordToFile(crossword, filename);
  std::cout << "Saved to '" << filename << "'." << std::endl;
}

void HandleLoad(std::unique_ptr<Crossword>* crossword_ptr) {
  const std::string filename = crossword::ui::ReadLinePrompt("Load filename: ");
  Crossword loaded = crossword::ui::LoadCrosswordFromFile(filename);
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

}  // namespace

int main() {
  crossword::ui::InitConsole();

  std::unique_ptr<Crossword> crossword_ptr;
  bool running = true;

  while (running) {
    try {
      PrintMenu();
      const size_t choice = crossword::ui::ReadSizeT("Select action: ");

      switch (choice) {
        case 1:
          HandleCreate(&crossword_ptr);
          break;
        case 2:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            HandleAddWord(crossword_ptr.get());
          }
          break;
        case 3:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            HandleRemoveWord(crossword_ptr.get());
          }
          break;
        case 4:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            HandleDisplay(*crossword_ptr);
          }
          break;
        case 5:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            HandleValidate(*crossword_ptr);
          }
          break;
        case 6:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            HandleSave(*crossword_ptr);
          }
          break;
        case 7:
          HandleLoad(&crossword_ptr);
          break;
        case 8:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            HandleSwitchMode(crossword_ptr.get(), CrosswordMode::kCreate);
          }
          break;
        case 9:
          if (EnsureCrosswordLoaded(crossword_ptr)) {
            HandleSwitchMode(crossword_ptr.get(), CrosswordMode::kSolve);
          }
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
