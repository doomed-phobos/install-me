#pragma once
#include "src/app_flags.hpp"

#include <memory>

namespace app {
   class AppCliCommands;

   class App {
   public:
      App(int argc, char* argv[]);
      ~App();

      /// Main function
      void run();
   private:
      // void copy_to(const fs::path& from, const fs::path& to, bool force) const;
      std::string m_inputDir;
      std::string m_outputDir;
      AppFlags m_flags;
      std::unique_ptr<AppCliCommands> m_acc;
   };
} // namespace app