#pragma once
#include "program_options.hpp"
#include "fwd.hpp"

#include <list>
#include <optional>
#include <functional>

namespace app {
  /// Manages CLI Commands
  class AppCliCommands {
  public:
    AppCliCommands();
    
    std::optional<PackageInfo> parse(int argc, char* argv[]);

    auto& getHelpCallback() {return m_help.callback;}
    auto& getShowListCallback() {return m_list.callback;}
    auto& getUninstallCallback() {return m_uninstall.callback;}
    const ProgramOptions& po() const {return m_po;}
  private:
    typedef std::function<void(const ProgramOptions::Option&)> Callback;

    struct CallableOption {
      ProgramOptions::Option& option;
      Callback callback;
        
      CallableOption(ProgramOptions::Option& option);
      ~CallableOption();

      static bool FindAndExecute(const ProgramOptions& po);
      
      static inline std::list<CallableOption*> s_opts;
    };

    void checkRequiredOptions();
    AppFlags parseFlags();

    ProgramOptions m_po;
    CallableOption m_uninstall;
    CallableOption m_list;
    CallableOption m_help;
    ProgramOptions::Option& m_name;
    ProgramOptions::Option& m_verbose;
    ProgramOptions::Option& m_nthreads;
    ProgramOptions::Option& m_inputDir;
    ProgramOptions::Option& m_outputDir; 
  };
} // namespace app