#include "src/app_cli_commands.hpp"

#include "src/app_flags.hpp"
#include "src/fs.hpp"

#include <algorithm>

namespace app {
   class MissingRequiredOption : public std::runtime_error {
   public:
      MissingRequiredOption(const Option& o) :
         std::runtime_error("'" + o.name() + "' option is required!") {}
   };
   
   AppCliCommands::UniqueOption::UniqueOption(Option& option, Callback&& callback) :
      m_option(option.setUniqueOption(true)),
      m_callback(std::move(callback)) {
      global_unique_opts.push_back(this);
   }
   AppCliCommands::UniqueOption::~UniqueOption() {
      global_unique_opts.erase(
         std::find_if(global_unique_opts.cbegin(), global_unique_opts.cend(), [this] (const UniqueOption* uo) -> bool {
            return this == uo;
         })
      );
   }

   void AppCliCommands::UniqueOption::setCallback(Callback&& callback) {
      m_callback = std::move(callback);
   }

   void AppCliCommands::UniqueOption::FindAndExecute(const PO& po) {
      for(const auto& option : global_unique_opts) {
         if(po.enabled(option->m_option) && option->m_callback != nullptr) {
            option->m_callback(option->m_option);
            exit(0);
         }
      }
   }

   AppCliCommands::AppCliCommands() :
      m_uninstall(m_po.add("uninstall").setAliasChr('u').setValueName("pkg_name").setDescription("Uninstall a package")),
      m_symLink(m_po.add("sym_link").setAliasChr('s').setDescription("Create symlinks instead copy files")),
      m_list(m_po.add("list").setAliasChr('l').setDescription("Show all installed packages")),
      m_verbose(m_po.add("verbose").setAliasChr('v').setDescription("Show what is going on.")),
      m_inputDir(m_po.add("input_dir").setAliasChr('i').setValueName("dir").setDescription("Directory where project that you want install is located.")),
      m_outputDir(m_po.add("output_dir").setAliasChr('o').setValueName("dir").setDescription("Directory where your project will install.")),
      m_help(m_po.add("help").setAliasChr('h').setDescription("Show this help list and exit.")) {
   }

   void AppCliCommands::setOnHelp(Callback&& callback) {m_help.setCallback(std::move(callback));}
   void AppCliCommands::setOnShowList(Callback&& callback) {m_list.setCallback(std::move(callback));}
   void AppCliCommands::setOnUninstall(Callback&& callback) {m_uninstall.setCallback(std::move(callback));}

   std::tuple<AppFlags, fs::path, fs::path> AppCliCommands::parse(int argc, char* argv[]) {
      m_po.parse(argc, argv);
      UniqueOption::FindAndExecute(m_po);

      checkRequiredOptions();
      
      return {parseFlags(),
         m_po.valueOf(m_inputDir),
         m_po.valueOf(m_outputDir)};
   }

   AppFlags AppCliCommands::parseFlags() {
      AppFlags flags;
      if(m_po.enabled(m_verbose))
         flags.addFlags(AppFlags::kVerbose);
      if(m_po.enabled(m_symLink))
         flags.addFlags(AppFlags::kSymLink);

      return flags;
   }

   void AppCliCommands::checkRequiredOptions() {
      if(!m_po.enabled(m_inputDir)) throw MissingRequiredOption(m_inputDir);
      if(!m_po.enabled(m_outputDir)) throw MissingRequiredOption(m_outputDir);
   }
} // namespace app