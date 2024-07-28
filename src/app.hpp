#pragma once
// #include "app_flags.hpp"
// #include "fs.hpp"
// #include "program_options.hpp"
#include "app_cli_commands.hpp"
#include "package_info.hpp"
#include "fwd.hpp"

#include <memory>

namespace app {
   class App {
   public:
      App(int argc, char* argv[]);

      /// Main function
      void run();
   private:
      void onHelp(const ProgramOptions::Option& option);
      void onShowList(const ProgramOptions::Option& option);
      void onUninstall(const ProgramOptions::Option& option);
      
      std::unique_ptr<AppCliCommands> m_acc;
      PackageInfo m_pack;
      bool m_shouldExit;
      PackageManager& m_pkgManager;
      /*
      fs::path m_inputDir;
      fs::path m_outputDir;
      AppFlags m_flags;
      std::optional<std::string> m_name;
      */
   };
} // namespace app