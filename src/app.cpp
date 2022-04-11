#include "src/app.hpp"

#include "src/app_cli_commands.hpp"
#include "src/output.hpp"

#include <filesystem>

namespace fs = std::filesystem;

class App::Output {
public:
   virtual void say(const std::string& msg) = 0;
};

class App::VerboseOutput : public App::Output {
public:
   virtual void say(const std::string& msg) override {
      out::info(msg);
   }
};

class App::NullOutput : public App::Output {
public:
   virtual void say(const std::string& msg) override {}
};

App::App(int argc, char* argv[]) : 
   m_acc(new AppCliCommands(argc, argv)) {
      
   if(m_acc->flags().hasFlags(AppFlags::kVerbose))
      m_out.reset(new VerboseOutput());
   else
      m_out.reset(new NullOutput());
}

App::~App() {}

void App::run() {
   std::error_code e;
   auto inputPath = fs::canonical(m_acc->inputDir(), e);
   if(e)
      throw AppException("Input directory: " + e.message());
   auto outputPath = fs::canonical(m_acc->outputDir(), e);
   if(e)
      throw AppException("Output directory: " + e.message());

   m_out->say("-----------------------------------------------");
   m_out->say("Input: " + inputPath.string());
   m_out->say("Output: " + outputPath.string());
   m_out->say("-----------------------------------------------");

   const AppFlags& flags = m_acc->flags();
   for(const auto& file0 : fs::directory_iterator(inputPath)) {
      if(file0.is_directory()) {
         auto it1 = fs::recursive_directory_iterator(file0.path());

         for(const auto& file1 : it1) {
            if(file1.is_directory()) {
               if(!flags.hasFlags(AppFlags::kRecursive))
                  it1.disable_recursion_pending();
            } else {
               // Copying to output
            }
         }
      } else if(flags.hasFlags(AppFlags::kIncludeAllFiles)) {
         // Copying to output
      }
   }
}