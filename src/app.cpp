#include "src/app.hpp"

#include "src/app_cli_commands.hpp"
#include "src/fs.hpp"
#include "src/output.hpp"

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

bool App::run() {
   std::string inputDir = utils::get_canonical_path(m_acc->inputDir());
   std::string outputDir = utils::get_canonical_path(m_acc->outputDir());
   if(!utils::is_directory(inputDir)) {
      out::error("Directory '" + inputDir + "' is not a directory");
      return false;
   }
   if(!utils::is_directory(outputDir)) {
      out::error("Directory '" + outputDir + "' is not a directory");
      return false;
   }

   m_out->say("----------------------------------------");
   m_out->say("Input directory: " + inputDir);
   m_out->say("Output directory: " + outputDir);
   m_out->say("----------------------------------------");

   return true;
}