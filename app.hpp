#pragma once
#include "program_options.hpp"

typedef ProgramOptions PO;
typedef PO::Option Option;
typedef PO::Value Value;

class App {
public:
   App(int argc, char* argv[]);

   int run();
private:
   void showHelp();

   PO m_po;
   Option& m_help;
   Option& m_force;
   Option& m_inputDir;
   Option& m_outputDir;
};