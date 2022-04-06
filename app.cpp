#include "app.hpp"

#include "generate_macros.hpp"

#include <stdexcept>
#include <iostream>

App::App(int argc, char* argv[]) :
   m_help(m_po.add("help").setAliasChr('?').setDescription("Show this help list and exit")) {
   try {
      m_po.parse(argc, argv);
   } catch(const std::runtime_error& e) {
      std::cerr << "[x] " << e.what() << "\n"
                << "    Try '" << PROJECT_NAME << " --help' for more information" << std::endl;
   }
}

int App::run() {
   if(m_po.enabled(m_help)) {
      showHelp();
      return 0;
   }

   return 0;
}

void App::showHelp() {
   
}