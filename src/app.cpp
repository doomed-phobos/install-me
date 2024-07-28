#include "app.hpp"

// #include "app_cli_commands.hpp"
#include "generate_macros.hpp"
#include "package.hpp"
#include "output.hpp"
#include "package_manager.hpp"
// #include "string.hpp"
// #include "parse_exception.hpp"
// #include "package_exception.hpp"

#include <iostream>

namespace app {
  App::App(int argc, char* argv[]) :
    m_acc{std::make_unique<AppCliCommands>()},
    m_shouldExit{},
    m_pkgManager{PackageManager::GetInstance()} {
    m_acc->getHelpCallback() = [this] (const auto& opt) {onHelp(opt);};
    m_acc->getShowListCallback() = [this] (const auto& opt) {onShowList(opt);};
    m_acc->getUninstallCallback() = [this] (const auto& opt) {onUninstall(opt);};

    if(auto res = m_acc->parse(argc, argv); res.has_value())
      m_pack = res.value();
    else
      m_shouldExit = true;
  }

  void App::run() {
    if(m_shouldExit)
      return;
    
    auto& pkg = m_pkgManager.createPackage(m_pack);
    if(!pkg.save().get())
      throw std::runtime_error(std::format("Failed to creating package '{}'", pkg.info().name));
    else
      WARNING("Package '{}' created successfully", pkg.info().name);
  }

  void App::onHelp(const ProgramOptions::Option& option) {
    std::cout << PROJECT_NAME << " " << VERSION << " by phobos\n\n"
              << "Usage: " << PROJECT_NAME << " [Options]\n"
              << m_acc->po();
  }

  void App::onShowList(const ProgramOptions::Option& option) {
    // std::cout << *m_pkgManager;
  }

  void App::onUninstall(const ProgramOptions::Option& option) {
    // std::string name = m_acc->po().valueOf(option);
    // try {
        // m_pkgManager->uninstallPackage(name);
    // } catch(const PackageException& e) {
        // ERROR("Uninstall failed: " + std::string(e.what()));
    // }
  }
  
  /*App::App(int argc, char* argv[]) :
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
  }*/
} // namespace app