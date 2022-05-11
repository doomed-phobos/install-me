#pragma once
#include "src/program_options.hpp"
#include "src/fwd.hpp"

#include <list>
#include <tuple>
#include <functional>

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

      std::tuple<AppFlags, fs::path, fs::path> parse(int argc, char* argv[]);

      const PO& po() const {return m_po;}
   private:
      class UniqueOption {
      public:
         UniqueOption(Option& option, Callback&& callback = nullptr);
         ~UniqueOption();

         void setCallback(Callback&& callback);

         static void FindAndExecute(const PO& po);
      private:
         static inline std::list<UniqueOption*> global_unique_opts;

         Option& m_option;
         Callback m_callback;
      };

      AppFlags parseFlags();
      void checkRequiredOptions();

      PO m_po;
      UniqueOption m_uninstall;
      UniqueOption m_list;
      UniqueOption m_help;
      Option& m_symLink;
      Option& m_verbose;
      Option& m_inputDir;
      Option& m_outputDir;
   };
} // namespace app