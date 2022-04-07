#include "app.hpp"

#include "app_cli_commands.hpp"

App::App(int argc, char* argv[]) : 
   m_acc(new AppCliCommands(argc, argv)) {
}

App::~App() {}

bool App::run() {
   if(!m_acc->isSuccess())   
      return false;

   const AppFlags& flags = m_acc->flags();

   return true;
}