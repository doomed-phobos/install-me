#include "app.hpp"

#include "generate_macros.hpp"
#include "package.hpp"
#include "output.hpp"
#include "package_manager.hpp"

#include <iostream>

namespace app {
  App::App(int argc, char* argv[]) :
    m_acc{std::make_unique<AppCliCommands>()},
    m_shouldExit{},
    m_pkgManager{PackageManager::GetInstance()} {
    m_acc->getHelpCallback() = [this] (const auto& opt) {onHelp(opt);};
    m_acc->getShowListCallback() = [this] (const auto& opt) {onShowList(opt);};
    m_acc->getUninstallCallback() = [this] (const auto& opt) {onUninstall(opt);};

    if(auto res = m_acc->parse(argc, argv); res.has_value()) {
      m_pack = res.value();

      m_pack.inputDir = utils::get_canonical(m_pack.inputDir);
      m_pack.outputDir = utils::get_canonical(m_pack.outputDir);
      
      if(m_pack.name.empty()) {
        m_pack.name = m_pack.inputDir.stem().string();
        WARNING("Package name is empty. Using default package name: '{}'", m_pack.name);
      }
    } else
      m_shouldExit = true;
  }

  void App::run() {
    if(m_shouldExit)
      return;
    
    if(fs::is_empty(m_pack.inputDir)) {
      ERROR("Input dir '{}' is empty", m_pack.inputDir.string());
      goto err;
    }

    if(!utils::create_if_not_exists(m_pack.outputDir)) {
      goto err;
    } else {
      WARNING("Output dir '{}' already exists.", m_pack.outputDir.string());
    }

    if(!m_pkgManager.installPackage(m_pack)) goto err;
    else WARNING("Package '{}' created successfully", m_pack.name);

    return;
    err:
      throw std::runtime_error(std::format("Failed to creating package '{}'", m_pack.name));
  }

  void App::onHelp(const ProgramOptions::Option& option) {
    std::cout << PROJECT_NAME << " " << VERSION << " by phobos\n\n"
              << "Usage: " << PROJECT_NAME << " [Options]\n"
              << m_acc->po();
  }

  void App::onShowList(const ProgramOptions::Option& option) {
    std::cout << m_pkgManager;
  }

  void App::onUninstall(const ProgramOptions::Option& option) {
    auto name = m_acc->po().valueOf(option);
    if(!m_pkgManager.uninstallPackage(name))
      throw std::runtime_error(std::format("Failed to uninstall '{}' package", name));
    else
      WARNING("Uninstalled '{}' package successfully", name);
  }
} // namespace app