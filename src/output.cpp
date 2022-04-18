#include "src/output.hpp"

#include <cassert>
#include <memory>

namespace app {
   static std::shared_ptr<Output> g_instance(new DefaultOutput());

    static void puts_with_sign(std::ostream& out, char sign, const std::string& msg) {
      if(sign == EOF || msg.empty())
         return;

      bool multiline = msg.find('\n') != std::string::npos;
      if(!multiline) {
         out << "[" << sign << "] " << msg << std::endl;
      } else {
         std::istringstream ss(msg);
         std::string line;
         if(std::getline(ss, line)) {
            out << "[" << sign << "] " << line << std::endl;
            while(std::getline(ss, line))
               out << "    " << line << std::endl;
         }
      }
   }

   Output* Output::Instance() {
      return g_instance.get();
   }

   void Output::SetInstance(Output* instance) {
      assert(instance);
      g_instance.reset(instance);
   }

   void DefaultOutput::info(const std::string& msg) {}
   void DefaultOutput::warning(const std::string& msg) {}
   void DefaultOutput::error(const std::string& msg) {
      puts_with_sign(std::cerr, 'x', msg + "\nAborting...");
   }

   void VerboseOutput::info(const std::string& msg) {
      puts_with_sign(std::cout, '*', msg);
   }

   void VerboseOutput::warning(const std::string& msg) {
      puts_with_sign(std::cerr, '!', msg);
   }

   void VerboseOutput::error(const std::string& msg) {
      DefaultOutput::error(msg);
   }
} // namespace app