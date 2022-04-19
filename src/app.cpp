#include "src/app.hpp"

#include "src/app_cli_commands.hpp"
#include "src/cache_manager.hpp"
#include "src/output.hpp"

namespace app {
   App::App(int argc, char* argv[]) : 
      m_acc(new AppCliCommands()) {
      
      std::tie(m_flags, m_inputDir, m_outputDir) = m_acc->parse(argc, argv).get();

      if(m_flags.hasFlags(AppFlags::kVerbose))
         Output::SetInstance(new VerboseOutput());
   }

   App::~App() {}

   void App::run() {
      /*m_acc->run();
      std::error_code e;
      std::string inputDir = m_acc->inputDir();
      auto it = fs::directory_iterator(inputDir, e);
      if(e)
         throw AppException("Input directory: " + e.message());

      std::string outputDir = m_acc->outputDir();
      if(!fs::is_directory(outputDir, e))
         throw AppException("Output directory: " + e.message());

      INFO("-----------------------------------------------");
      INFO("Input: " + inputDir);
      INFO("Output: " + outputDir);
      INFO("-----------------------------------------------");

      const AppFlags& flags = m_acc->flags();
      bool force = flags.hasFlags(AppFlags::kForce);
      for(const auto& file0 : it) {
         if(file0.is_directory()) {
            auto it1 = fs::recursive_directory_iterator(file0.path());

            for(const auto& file1 : it1) {
               if(file1.is_directory()) {
                  if(!flags.hasFlags(AppFlags::kRecursive))
                     it1.disable_recursion_pending();
               } else {
                  copy_to(file1, outputDir, force);
               }
            }
         } else if(flags.hasFlags(AppFlags::kIncludeAllFiles)) {
            // copy_to(file0.path().filename(), outputDir, force);
            copy_to(file0.path().filename(), outputDir, force);
         }
      }
   }

   void App::copy_to(const fs::path& from, const fs::path& to, bool force) const {
      // fs::path final_to = to/from.parent_path().stem()/from.filename(); 
      // FIXME: Fix recursive directory error
      //          my_input_dir/lib/version 2
      //          -> returns "version 2" instead "lib/version 2"
      std::cout << from.parent_path().root_name() << std::endl;
      std::cout << from.parent_path().root_directory() << std::endl;
      std::cout << from.parent_path().root_path() << std::endl;
      std::cout << from.parent_path().relative_path() << std::endl;
      std::cout << from.parent_path().parent_path() << std::endl;
      std::cout << from.parent_path().filename() << std::endl;
      std::cout << from.parent_path().stem() << std::endl;
      std::cout << from.parent_path().extension() << "\n\n";
      // m_out->say("File: " + from.string());
      // m_out->say(final_to.string());*/
   }
} // namespace app