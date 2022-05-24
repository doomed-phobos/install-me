#include "src/app.hpp"

#include "src/app_cli_commands.hpp"
#include "src/output.hpp"
#include "src/package.hpp"
#include "src/package_manager.hpp"
#include "src/generate_macros.hpp"
#include "src/string.hpp"
#include "src/parse_exception.hpp"
#include "src/package_exception.hpp"

#define BIND_MEMBER_FUNCTION(foo) [this] (const ProgramOptions::Option& option) {this->foo(option);}

namespace app {
   App::App(int argc, char* argv[]) :
      m_acc(new AppCliCommands()),
      m_exit(false) {
      m_acc->setOnHelp(BIND_MEMBER_FUNCTION(onHelp));
      m_acc->setOnShowList(BIND_MEMBER_FUNCTION(onShowList));
      m_acc->setOnUninstall(BIND_MEMBER_FUNCTION(onUninstall));

      m_pkgManager = PackageManager::GetInstance();
      auto result = m_acc->parse(argc, argv);
      if(!result.has_value()) {
         m_exit = true;
      } else {
         std::tie(m_flags, m_inputDir, m_outputDir, m_name) = *result;
      }
   }

   App::~App() {}

   void App::run() {
      if(m_exit)
         return;

      bool symlink = m_flags.hasFlags(AppFlags::kSymLink);
      m_inputDir = utils::get_canonical(m_inputDir);
      VERBOSE("Canonical input dir: " + m_inputDir.string());
       
      const char* pkgName = nullptr;
      if(m_name.has_value())
         pkgName = &(*m_name)[0];
      Package& pkg = m_pkgManager->createPackageFromDirectory(m_inputDir, m_outputDir, symlink, pkgName);
      INFO(utils::fmt_to_str("Package '%s' copied to '%s' successfully!", m_inputDir.c_str(), m_outputDir.c_str()));
   }

   void App::onHelp(const Option& option) {
      std::cout << PROJECT_NAME << " " << VERSION << " by phobos\n\n"
               << "Usage: " << PROJECT_NAME << " [Options]\n"
               << m_acc->po();
   }

   void App::onShowList(const Option& option) {
      std::cout << *m_pkgManager;
   }

   void App::onUninstall(const Option& option) {
      std::string name = m_acc->po().valueOf(option);
      try {
         m_pkgManager->uninstallPackage(name);
      } catch(const PackageException& e) {
         ERROR("Uninstall failed: " + std::string(e.what()));
      }
   }
} // namespace app