#include "app_cli_commands.hpp"

#include "parse_exception.hpp"
#include "output.hpp"

#include <algorithm>

namespace app {
  class MissingRequiredOption : public ParseException {
  public:
    MissingRequiredOption(const ProgramOptions::Option& opt) :
      ParseException(std::format("Option known as '{}' is required", opt.name())) {}
  };

  AppCliCommands::CallableOption::CallableOption(ProgramOptions::Option& option) :
    option{option} {
    s_opts.push_back(this);
  }

  AppCliCommands::CallableOption::~CallableOption() {
    s_opts.erase(std::ranges::find(s_opts, this));
  }

  bool AppCliCommands::CallableOption::FindAndExecute(const ProgramOptions& po) {
    for(const auto& option : s_opts) {
      if(po.enabled(option->option) && option->callback) {
        option->callback(option->option);
        return true;
      }
    }

    return false;
  }

  AppCliCommands::AppCliCommands() :
    m_uninstall(m_po.add("uninstall").setAliasChr('u').setValueName("pkg_name").setDescription("Uninstall a package.")),
    m_symLink(m_po.add("sym_link").setAliasChr('s').setDescription("Create symlinks instead copy files.")),
    m_list(m_po.add("list").setUniqueOption(true).setAliasChr('l').setDescription("Show all installed packages.")),
    m_verbose(m_po.add("verbose").setAliasChr('v').setDescription("Show what is going on.")),
    m_name(m_po.add("name").setAliasChr('n').setValueName("pkg_name").setDescription("Set custom package name. Default is <input_dir> directory name.")),
    m_inputDir(m_po.add("input_dir").setAliasChr('i').setValueName("dir").setDescription("Directory where project that you want install is located.")),
    m_outputDir(m_po.add("output_dir").setAliasChr('o').setValueName("dir").setDescription("Directory where your project will install.")),
    m_help(m_po.add("help").setUniqueOption(true).setAliasChr('?').setDescription("Show this help list.")) {}

  std::optional<AppCliCommands::ParseResult> AppCliCommands::parse(int argc, char* argv[]) {
    m_po.parse(argc, argv);
    
    if(m_po.enabled(m_verbose))
        out::allow_verbose = true;

    if(CallableOption::FindAndExecute(m_po))
        return {};
    
    checkRequiredOptions();
    
    std::optional<std::string> name{};
    if(m_po.enabled(m_name))
        name = m_po.valueOf(m_name);

    // return std::tuple<AppFlags, fs::path, fs::path, std::optional<std::string>>(
    //    parseFlags(),
    //    m_po.valueOf(m_inputDir),
    //    m_po.valueOf(m_outputDir),
    //    name)};
    return {{parseFlags(), m_po.valueOf(m_inputDir), m_po.valueOf(m_outputDir)}};
  }

  void AppCliCommands::checkRequiredOptions() {
    if(!m_po.enabled(m_inputDir)) throw MissingRequiredOption(m_inputDir);
    if(!m_po.enabled(m_outputDir)) throw MissingRequiredOption(m_outputDir);
  }

  AppFlags AppCliCommands::parseFlags() {
    AppFlags flags;
    if(m_po.enabled(m_symLink))
        flags.addFlags(AppFlags::kSymLink);

    return flags;
  }
} // namespace app