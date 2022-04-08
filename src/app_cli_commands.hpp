#pragma once
#include "src/program_options.hpp"
#include "src/app_flags.hpp"

typedef ProgramOptions PO;
typedef PO::Option Option;
typedef PO::Value Value;

class AppCliCommands {
public:
   AppCliCommands(int argc, char* argv[]);

   const AppFlags& flags() const;
   std::string inputDir() const;
   std::string outputDir() const;
private:
   void showHelp();
   void parseFlags();
   void checkRequiredOptions();

   PO m_po;
   Option& m_verbose;
   Option& m_includeAllFiles;
   Option& m_recursive;
   Option& m_help;
   Option& m_force;
   Option& m_inputDir;
   Option& m_outputDir;
   AppFlags m_flags;
};