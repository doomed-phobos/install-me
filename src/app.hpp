#pragma once
#include <memory>
#include <stdexcept>

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

   void copy_to(const std::string& from, const std::string& to, bool force);

   std::unique_ptr<AppCliCommands> m_acc;
   std::unique_ptr<Output> m_out;
};