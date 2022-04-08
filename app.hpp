#pragma once
#include <memory>

class AppCliCommands;

class App {
public:
   App(int argc, char* argv[]);
   ~App();

   bool run();
private:
   class Output;
   class VerboseOutput;
   class NullOutput;

   std::unique_ptr<AppCliCommands> m_acc;
   std::unique_ptr<Output> m_out;
};