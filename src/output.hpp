#pragma once
#include <iostream>
#include <iomanip>

namespace out {
   namespace priv {
      /// Support multiline
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
   }

   inline void info(const std::string& msg) {
      priv::puts_with_sign(std::cout, '*', msg);
   }

   inline void warning(const std::string& msg) {
      priv::puts_with_sign(std::cerr, '!', msg);
   }

   inline void error(const std::string& msg) {
      priv::puts_with_sign(std::cerr, 'x', msg + "\nAborting...");
   }
} // namespace out