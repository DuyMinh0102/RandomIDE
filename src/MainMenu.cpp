#include "MainMenu.h"
#include "Runner.h"
#include "globals.h"
#include "imgui.h"
#include "portable-file-dialogs.h"
#include <filesystem>
#include <fstream>

using namespace ImGui;

void MainMenuBarFunc(GLFWwindow *window) {
  ImGuiIO &io = GetIO();

  if (BeginMainMenuBar()) {
    std::string activeFile = "";
    std::string ext = "";
    bool canRun = false;

    if (activeTabIndex >= 0 && activeTabIndex < (int)openTabs.size()) {
      activeFile = openTabs[activeTabIndex]->filepath.string();
      ext = openTabs[activeTabIndex]->filepath.extension().string();
      canRun = !activeFile.empty() && isRunnableLanguage(ext);
    }

    if (BeginMenu("File")) {
      if (MenuItem("Open", "Ctrl + O")) {
        auto selection =
            pfd::open_file("Open File", ".",
                           {"C++ Files", "*.cpp *.h *.hpp", "All Files", "*"})
                .result();

        if (!selection.empty()) {
          std::filesystem::path objPath(selection[0]);
          bool alreadyOpen = false;
          int existingTabIndex = -1;

          for (size_t i = 0; i < openTabs.size(); i++) {
            if (openTabs[i]->filepath == objPath) {
              alreadyOpen = true;
              existingTabIndex = i;
              break;
            }
          }

          if (alreadyOpen) {
            activeTabIndex = existingTabIndex;
          } else {
            openTabs.push_back(std::make_unique<Tab>(objPath));
            activeTabIndex = openTabs.size() - 1;
          }

          current_path = objPath.parent_path();
          showWelcomeScreen = false;
        }
      }
      if (MenuItem("Save", "Ctrl + S")) {
        if (activeTabIndex >= 0 && activeTabIndex < (int)openTabs.size()) {
          auto &tab = openTabs[activeTabIndex];
          std::string currentText = tab->editor.GetText();

          if (tab->filepath.empty()) {
            auto destination =
                pfd::save_file("Save File", ".", {"All Files", " * "}).result();

            if (!destination.empty()) {
              tab->filepath = destination;
              std::ofstream outFile(destination);
              if (outFile.is_open()) {
                outFile << currentText;
                outFile.close();
                tab->is_modified = false;
              }
            }
          } else {
            std::ofstream outFile(tab->filepath);
            if (outFile.is_open()) {
              outFile << currentText;
              outFile.close();
              tab->is_modified = false;
            }
          }
        }
      }
      if (MenuItem("Save As", "Ctrl + Shift + S")) {
        if (activeTabIndex >= 0 && activeTabIndex < (int)openTabs.size()) {
          auto &tab = openTabs[activeTabIndex];
          std::string currentText = tab->editor.GetText();
          auto destination =
              pfd::save_file("Save File", ".", {"All Files", "*"}).result();

          if (!destination.empty()) {
            tab->filepath = destination;
            std::ofstream outFile(destination);
            if (outFile.is_open()) {
              outFile << currentText;
              outFile.close();
              tab->is_modified = false;
            }
          }
        }
      }
      Separator();
      if (MenuItem("Exit", "Alt+F4")) {
        glfwSetWindowShouldClose(window, true);
      }
      EndMenu();
    }

    if (BeginMenu("View")) {
      Separator();

      if (MenuItem("Zoom In", "Ctrl + =")) {
        if (io.FontGlobalScale < 4.0f) {
          io.FontGlobalScale += 0.1f;
          separatorPos = baseSeparatorPos * io.FontGlobalScale;
        }
      }
      if (MenuItem("Zoom Out", "Ctrl + -")) {
        if (io.FontGlobalScale > 1.0f) {
          io.FontGlobalScale -= 0.1f;
          separatorPos = baseSeparatorPos * io.FontGlobalScale;
        }
      }

      EndMenu();
    }

    if (MenuItem("Sidebar")) {
      showSidebar = !showSidebar;
    }
    if (MenuItem("Terminal")) {
      showTerminal = !showTerminal;
    }

    std::string runLabel =
        (canRun ? "Run with " + languageName(ext) : "Run Code");

    float itemWidth =
        CalcTextSize(runLabel.c_str()).x + GetStyle().FramePadding.x * 2.0f;

    float rightXPosition =
        GetCursorPosX() + GetContentRegionAvail().x - itemWidth;

    SetCursorPosX(rightXPosition);

    if (MenuItem(runLabel.c_str())) {
      if (canRun) {
        RunCurrentFile(current_path);
      }
    }

    if (io.KeyCtrl) {
      if (IsKeyPressed(ImGuiKey_Equal)) {
        if (io.FontGlobalScale < 4.0f) {
          io.FontGlobalScale += 0.1f;
          separatorPos = baseSeparatorPos * io.FontGlobalScale;
        }
      }
      if (IsKeyPressed(ImGuiKey_Minus)) {
        if (io.FontGlobalScale > 1.0f) {
          io.FontGlobalScale -= 0.1f;
          separatorPos = baseSeparatorPos * io.FontGlobalScale;
        }
      }
      if (IsKeyPressed(ImGuiKey_0)) {
        io.FontGlobalScale = 1.0f;
        separatorPos = 250.0f;
      }

      if (IsKeyPressed(ImGuiKey_O)) {
        auto selection =
            pfd::open_file("Open File", ".",
                           {"C++ Files", "*.cpp *.h *.hpp", "All Files", "*"})
                .result();

        if (!selection.empty()) {
          std::filesystem::path objPath(selection[0]);
          bool alreadyOpen = false;
          int existingTabIndex = -1;

          for (size_t i = 0; i < openTabs.size(); i++) {
            if (openTabs[i]->filepath == objPath) {
              alreadyOpen = true;
              existingTabIndex = i;
              break;
            }
          }

          if (alreadyOpen) {
            activeTabIndex = existingTabIndex;
          } else {
            openTabs.push_back(std::make_unique<Tab>(objPath));
            activeTabIndex = openTabs.size() - 1;
          }

          current_path = objPath.parent_path();
          showWelcomeScreen = false;
        }
      }
      if (IsKeyPressed(ImGuiKey_S)) {
        if (activeTabIndex >= 0 && activeTabIndex < (int)openTabs.size()) {
          std::string currentText = openTabs[activeTabIndex]->editor.GetText();
          std::ofstream outFile(openTabs[activeTabIndex]->filepath);
          if (outFile.is_open()) {
            outFile << currentText;
            outFile.close();
            openTabs[activeTabIndex]->is_modified = false;
          }
        }
      }
    }
    if (io.KeyAlt) {
      if (IsKeyPressed(ImGuiKey_F4))
        glfwSetWindowShouldClose(window, true);
    }

    if (IsKeyPressed(ImGuiKey_F5) && canRun) {
      RunCurrentFile(activeFile);
    }

    EndMainMenuBar();
  }
}
