#pragma once
#include <memory>

class AppCliCommands;

class App {
public:
   App(int argc, char* argv[]);
   ~App();

   bool run();
private:
   std::unique_ptr<AppCliCommands> m_acc;
};