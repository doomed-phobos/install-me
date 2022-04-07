#pragma once
#include "program_options.hpp"
#include "app_flags.hpp"

typedef ProgramOptions PO;
typedef PO::Option Option;
typedef PO::Value Value;

class AppCliCommands {
public:
   AppCliCommands(int argc, char* argv[]);

   const AppFlags& flags() const;
   bool isSuccess() const;
private:
   void showHelp();
   void parseFlags();
   void fail();

   PO m_po;
   Option& m_includeAllFiles;
   Option& m_recursive;
   Option& m_help;
   Option& m_force;
   Option& m_inputDir;
   Option& m_outputDir;
   AppFlags m_flags;
   bool m_success;
};