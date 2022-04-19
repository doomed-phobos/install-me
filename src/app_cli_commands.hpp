#pragma once
#include "src/program_options.hpp"
#include "src/app_flags.hpp"

#include <memory>
#include <tuple>

namespace app {
   typedef ProgramOptions PO;
   typedef PO::Option Option;
   typedef PO::Value Value;

   class CacheManager;

   /// Manages CLI Commands
   class AppCliCommands {
   public:
      AppCliCommands();

      utils::Expected<std::tuple<AppFlags, std::string, std::string>> parse(int argc, char* argv[]);
   private:
      void showHelp();
      AppFlags parseFlags();
      utils::Expected<void> checkRequiredOptions();

      PO m_po;
      Option& m_verbose;
      Option& m_includeAllFiles;
      Option& m_recursive;
      Option& m_help;
      Option& m_force;
      Option& m_inputDir;
      Option& m_outputDir;
   };
} // namespace app