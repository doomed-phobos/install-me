#include "src/app.hpp"

#include "src/app_cli_commands.hpp"
#include "src/output.hpp"
#include "src/package.hpp"
#include "src/package_manager.hpp"
#include "src/generate_macros.hpp"

// TODO: 12/05/2020 (IMPORTANT!!!) Create a request yes/no

#define BIND_MEMBER_FUNCTION(foo) [this] (const ProgramOptions::Option& option) {this->foo(option);}

namespace app {
   App::App(int argc, char* argv[]) :
      m_acc(new AppCliCommands()),
      m_pkgManager(PackageManager::GetInstance()) {
      m_acc->setOnHelp(BIND_MEMBER_FUNCTION(onHelp));
      m_acc->setOnShowList(BIND_MEMBER_FUNCTION(onShowList));
      m_acc->setOnUninstall(BIND_MEMBER_FUNCTION(onUninstall));

      std::tie(m_flags, m_inputDir, m_outputDir) = m_acc->parse(argc, argv);

      if(m_flags.hasFlags(AppFlags::kVerbose))
         Output::SetInstance(new VerboseOutput());
   }

   App::~App() {}
   // FIXME 08/05/2022: Añadir modo force para copiar incluso si tiene el mismo nombre que un paquete existente
   //                   Añadir desinstalación de paquetes

   void App::run() {
      m_inputDir = utils::get_canonical(m_inputDir);
      Package pkg(m_inputDir, m_outputDir, m_flags.hasFlags(AppFlags::kSymLink));
      if(m_pkgManager->hasPackage(pkg.pkgName()))
         throw std::runtime_error("Package '" + pkg.pkgName() + "' exists");

      for(const auto& path : fs::recursive_directory_iterator(m_inputDir))
         pkg.copyFrom(path);

      m_pkgManager->savePackage(std::move(pkg));
   }

   void App::onHelp(const Option& option) {
      std::cout << PROJECT_NAME << " " << VERSION << " by phobos\n\n"
               << "Usage: " << PROJECT_NAME << " [Options]\n"
               << m_acc->po()
      #if 0
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
      #else
      ;
      #endif
   }

   void App::onShowList(const Option& option) {
      std::cout << *m_pkgManager;
   }

   void App::onUninstall(const Option& option) {
      std::cout << "Package name: " << m_acc->po().valueOf(option) << std::endl;
   }
} // namespace app