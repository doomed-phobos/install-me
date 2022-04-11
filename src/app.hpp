#pragma once
#include <memory>
#include <stdexcept>
#include <filesystem>

class AppCliCommands;

class AppException : public std::runtime_error {
public:
   AppException(const std::string& msg) :
      std::runtime_error(msg) {}
};

class App {
public:
   App(int argc, char* argv[]);
   ~App();

   void run();
private:
   class Output;
   class VerboseOutput;
   class NullOutput;

   void copy_to(const std::filesystem::path& from, const std::filesystem::path& to, bool force) const;

   std::unique_ptr<AppCliCommands> m_acc;
   std::unique_ptr<Output> m_out;
};