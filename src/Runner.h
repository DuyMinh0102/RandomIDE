#pragma once
#include <string>

struct RunConfig {
  std::string name;
  std::string compileCMD;
  std::string runCMD;
};

void RunCurrentFile(const std::string &currentFilePath);

bool isRunnableLanguage(const std::string &extension);
std::string languageName(const std::string &extension);
