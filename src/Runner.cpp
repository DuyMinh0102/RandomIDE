#include "Runner.h"
#include <cstdlib>
#include <filesystem>
#include <unordered_map>

static const std::unordered_map<std::string, RunConfig> languageRegistry = {
    {".py", {"Python", "", "python3 \"%f\""}},
    {".js", {"JavaScript", "", "node \"%f\""}},
    {".cpp", {"C++", "g++ \"%f\" -o \"%n\"", "\"./%n\""}},
    {".c", {"C", "gcc \"%f\" -o \"%n\"", "\"./%n\""}}};

bool isRunnableLanguage(const std::string &extension) {
  return languageRegistry.find(extension) != languageRegistry.end();
}

std::string languageName(const std::string &extension) {
  auto it = languageRegistry.find(extension);
  return (it != languageRegistry.end()) ? it->second.name : "";
}

void RunCurrentFile(const std::string &currentFilePath) {
  if (currentFilePath.empty())
    return;

  std::filesystem::path p(currentFilePath);
  std::string ext = p.extension().string();

  auto it = languageRegistry.find(ext);
  if (it == languageRegistry.end())
    return;

  const RunConfig &config = it->second;
  std::string fullPath = p.string();
  std::string noExtPath = p.parent_path().string() + "/" + p.stem().string();

  auto replacePlaceholders = [&](std::string cmd) {
    size_t pos;
    while ((pos = cmd.find("%f")) != std::string::npos)
      cmd.replace(pos, 2, fullPath);
    while ((pos = cmd.find("%n")) != std::string::npos)
      cmd.replace(pos, 2, noExtPath);
    return cmd;
  };

  if (!config.compileCMD.empty()) {
    std::string finalCompile = replacePlaceholders(config.compileCMD);
    if (std::system(finalCompile.c_str()) != 0)
      return;
  }

  std::string finalRun = replacePlaceholders(config.compileCMD) + " &";
  std::system(finalRun.c_str());
}
