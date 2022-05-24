#pragma once
#include "src/app_flags.hpp"
#include "src/fs.hpp"
#include "src/fwd.hpp"
#include "src/program_options.hpp"

#include <optional>
#include <memory>

namespace app {
   class App {
   public:
      App(int argc, char* argv[]);
      ~App();

      /// Main function
      void run();
   private:
      void onHelp(const ProgramOptions::Option& option);
      void onShowList(const ProgramOptions::Option& option);
      void onUninstall(const ProgramOptions::Option& option);

      bool m_exit;
      fs::path m_inputDir;
      fs::path m_outputDir;
      AppFlags m_flags;
      std::optional<std::string> m_name;
      std::unique_ptr<AppCliCommands> m_acc;
      PackageManager* m_pkgManager;
   };
} // namespace app