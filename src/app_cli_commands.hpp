#pragma once
#include "src/program_options.hpp"
#include "src/fwd.hpp"

#include <list>
#include <tuple>
#include <functional>
#include <optional>

namespace app {
   typedef ProgramOptions PO;
   typedef PO::Option Option;
   typedef PO::Value Value;

   /// Manages CLI Commands
   class AppCliCommands {
   public:
      typedef std::function<void(const Option&)> Callback;

      AppCliCommands();
      
      void setOnHelp(Callback&& callback);
      void setOnShowList(Callback&& callback);
      void setOnUninstall(Callback&& callback);

      std::optional<
         std::tuple<AppFlags, fs::path, fs::path, std::optional<std::string>>> parse(int argc, char* argv[]);
      
      const PO& po() const {return m_po;}
   private:
      class CallableOption {
      public:
         CallableOption(Option& option, Callback&& callback = nullptr);
         ~CallableOption();

         void setCallback(Callback&& callback);

         static bool FindAndExecute(const PO& po);
      private:
         static inline std::list<CallableOption*> global_unique_opts;

         Option& m_option;
         Callback m_callback;
      };

      AppFlags parseFlags();
      void checkRequiredOptions();

      PO m_po;
      CallableOption m_uninstall;
      CallableOption m_list;
      CallableOption m_help;
      Option& m_name;
      Option& m_symLink;
      Option& m_verbose;
      Option& m_inputDir;
      Option& m_outputDir;
   };
} // namespace app