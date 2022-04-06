#include "app.hpp"

#include "generate_macros.hpp"

#include <stdexcept>
#include <iostream>

App::App(int argc, char* argv[]) :
   m_force(m_po.add("force").setAliasChr('f').setDescription("Create missing directories")),
   m_inputDir(m_po.add("input_dir").setAliasChr('i').setValueName("dir").setDescription("Directory where project that you want install is located")),
   m_outputDir(m_po.add("output_dir").setAliasChr('o').setValueName("dir").setDescription("Directory where your project will install")),
   m_help(m_po.add("help").setAliasChr('?').setDescription("Show this help list and exit")) {
   if(argc == 1)
      showHelp();
   
   try {
      m_po.parse(argc, argv);
   } catch(const std::runtime_error& e) {
      std::cerr << "[x] " << e.what() << "\n"
                << "    Try '" << PROJECT_NAME << " --help' or '" << PROJECT_NAME << " -?' instead for more information" << std::endl;
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
   std::cout << PROJECT_NAME << " " << VERSION << " by phobos\n\n"
             << "Usage: " << PROJECT_NAME << " [Options]\n\n"
             << "OPTIONS:\n"
             << m_po
             << "HOW WORKS:\n"
             << "  '" << PROJECT_NAME << "' will follow the <input_dir> structure for copy files to <output_dir>, e.g: \n"
             << "  - my_input_dir\n"
                "  |\n"
                "  --- bin\n"
                "  |   |\n"
                "  |   --- an_exe\n"
                "  |\n"
                "  --- lib\n"
                "      |\n"
                "      --- my_lib.jar\n";
   std::cout << "\n";
}