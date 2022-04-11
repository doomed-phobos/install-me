#include "src/app_cli_commands.hpp"

#include "src/generate_macros.hpp"
#include "src/output.hpp"
#include "src/string.hpp"

#include <stdexcept>

class AppCliInitException : public std::runtime_error {
public:
   AppCliInitException(const std::string& msg) :
      std::runtime_error(msg) {}
};

AppCliCommands::AppCliCommands(int argc, char* argv[]) :
   m_verbose(m_po.add("verbose").setAliasChr('v').setDescription("Show what is going on.")),
   m_includeAllFiles(m_po.add("include-all-files").setAliasChr('a').setDescription("Includes 'first layer' files (see HOW IT WORKS below).")),
   m_force(m_po.add("force").setAliasChr('f').setDescription("Create missing directories.")),
   m_inputDir(m_po.add("input_dir").setAliasChr('i').setValueName("dir").setDescription("Directory where project that you want install is located.")),
   m_outputDir(m_po.add("output_dir").setAliasChr('o').setValueName("dir").setDescription("Directory where your project will install.")),
   m_help(m_po.add("help").setAliasChr('h').setDescription("Show this help list and exit.")),
   m_recursive(m_po.add("recursive").setAliasChr('r').setDescription("Copy recursive files from found directories in <input_dir> to <output_dir>.\n"
                                                                     "Without 'recursive mode', it will just copy the 'first layer' (see HOW IT WORKS below).")) {
   if(argc == 1)
      showHelp();

   try {
      m_po.parse(argc, argv);
      parseFlags();
      
      if(m_po.enabled(m_help))
         showHelp();
      
      checkRequiredOptions();
   } catch(const ParseException& e) {
      out::error(utils::fmt_to_str("%s\n"
                                   "Try '%s --help' or '%s -?' instead for mode information\n", e.what(), PROJECT_NAME, PROJECT_NAME));
      exit(1);
   } catch(const AppCliInitException& e) {
      out::error(e.what());
      exit(1);
   }
}

const AppFlags& AppCliCommands::flags() const {return m_flags;}
std::string AppCliCommands::inputDir() const {return m_po.valueOf(m_inputDir);}
std::string AppCliCommands::outputDir() const {return m_po.valueOf(m_outputDir);}

void AppCliCommands::showHelp() {
   std::cout << PROJECT_NAME << " " << VERSION << " by phobos\n\n"
             << "Usage: " << PROJECT_NAME << " [Options]\n\n"
             << "OPTIONS:\n"
             << m_po
             << "HOW IT WORKS:                                                \n"
                " RECURSIVE MODE:                                             \n"
                "  *) Without it                                              \n"
                "     - my_input_dir               - my_output_dir            \n"
                "     |                            |                          \n"
                "     --- bin                      --- bin                    \n"
                "     |   |                        |   |                      \n"
                "     |   --- something            |   --- something          \n"
                "     |                            |                          \n"
                "     --- include                  --- include                \n"
                "     |   |                        |   |                      \n"
                "     |   --- cpp                  |   --- something.hpp      \n"
                "     |   |   |                    |                          \n"
                "     |   |   --- header.hpp       --- lib                    \n"
                "     |   |                        |   |                      \n"
                "     |   --- something.hpp        |   --- default.jar        \n"
                "     |                            |                          \n"
                "     --- lib                      --- src                    \n"
                "     |   |                            |                      \n"
                "     |   --- version_1                --- something.cpp      \n"
                "     |   |   |                                               \n"
                "     |   |   --- something.jar                               \n"
                "     |   --- version_2                                       \n"
                "     |   |   |                                               \n"
                "     |   |   --- something.jar                               \n"
                "     |   |                                                   \n"
                "     |   --- default.jar                                     \n"
                "     --- src                                                 \n"
                "         |                                                   \n"
                "         --- something.cpp                                   \n"
                "                                                             \n"
                "  *) With it                                                 \n"
                "     - my_input_dir               - my_output_dir            \n"
                "     |                            |                          \n"
                "     --- bin                      --- bin                    \n"
                "     |   |                        |   |                      \n"
                "     |   --- something            |   --- something          \n"
                "     |                            |                          \n"
                "     --- include                  --- include                \n"
                "     |   |                        |   |                      \n"
                "     |   --- cpp                  |   --- cpp                \n"
                "     |   |   |                    |   |   |                  \n"
                "     |   |   --- header.hpp       |   |   --- header.hpp     \n"
                "     |   |                            |                      \n"
                "     |   --- something.hpp        |   --- something.hpp      \n"
                "     |                            |                          \n"
                "     --- lib                      --- lib                    \n"
                "     |   |                        |   |                      \n"
                "     |   --- version_1            |   --- version_1          \n"
                "     |   |   |                    |   |    |                 \n"
                "     |   |   --- something.jar    |   |    --- something.jar \n"
                "     |   --- version_2            |   --- version_2          \n"
                "     |   |   |                    |   |   |                  \n"
                "     |   |   --- something.jar    |   |   --- something.jar  \n"
                "     |   |                        |   |                      \n"
                "     |   --- default.jar          |   --- default.jar        \n"
                "     |                            |                          \n"
                "     --- src                      --- src                    \n"
                "         |                            |                      \n"
                "         --- something.cpp            --- something.cpp      \n"
                "                                                             \n"
                " INCLUDE ALL FILES MODE:                                     \n"
                "  *) Without it                                              \n"
                "     - my_input_dir               - my_output_dir            \n"
                "     |                            |                          \n"
                "     --- bin                      --- bin                    \n"
                "     |   |                        |   |                      \n"
                "     |   --- something            |   --- something          \n"
                "     |                            |                          \n"
                "     --- include                  --- include                \n"
                "     |   |                        |   |                      \n"
                "     |   --- something.hpp        |   --- something.hpp      \n"
                "     |                            |                          \n"
                "     --- lib                      --- lib                    \n"
                "     |   |                        |   |                      \n"
                "     |   --- default.jar          |   --- default.jar        \n"
                "     |                            |                          \n"
                "     --- src                      --- src                    \n"
                "     |   |                            |                      \n"
                "     |   --- something.cpp            --- something.cpp      \n"
                "     |                                                       \n"
                "     --- something else.txt                                  \n"
                "     --- something else (1).txt                              \n"
                "                                                             \n"
                "  *) With it                                                 \n"
                "     - my_input_dir               - my_output_dir            \n"
                "     |                            |                          \n"
                "     --- bin                      --- bin                    \n"
                "     |   |                        |   |                      \n"
                "     |   --- something            |   --- something          \n"
                "     |                            |                          \n"
                "     --- include                  --- include                \n"
                "     |   |                        |   |                      \n"
                "     |   --- something.hpp        |   --- something.hpp      \n"
                "     |                            |                          \n"
                "     --- lib                      --- lib                    \n"
                "     |   |                        |   |                      \n"
                "     |   --- default.jar          |   --- default.jar        \n"
                "     |                            |                          \n"
                "     --- src                      --- src                    \n"
                "     |   |                        |   |                      \n"
                "     |   --- something.cpp        |   --- something.cpp      \n"
                "     |                            |                          \n"
                "     --- something else.txt       --- something else.txt     \n"
                "     --- something else (1).txt   --- something else (1).txt \n";
   std::cout << "\n";

   exit(1);
}

void AppCliCommands::parseFlags() {
   if(m_po.enabled(m_recursive))
      m_flags.addFlags(AppFlags::kRecursive);
   if(m_po.enabled(m_force))
      m_flags.addFlags(AppFlags::kForce);
   if(m_po.enabled(m_includeAllFiles))
      m_flags.addFlags(AppFlags::kIncludeAllFiles);
   if(m_po.enabled(m_verbose))
      m_flags.addFlags(AppFlags::kVerbose);
}

void AppCliCommands::checkRequiredOptions() {
   if(!m_po.enabled(m_inputDir))
      throw AppCliInitException("'" + m_inputDir.name() + "' option is required");

   if(!m_po.enabled(m_outputDir))
      throw AppCliInitException("'" + m_outputDir.name() + "' option is required");
}