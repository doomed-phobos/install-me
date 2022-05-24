#include "src/app.hpp"
#include "src/parse_exception.hpp"
#include "src/output.hpp"
#include "src/string.hpp"
#include "src/generate_macros.hpp"

int main(int argc, char* argv[]) {
   try {
      app::App app(argc, argv);
      app.run();

      return 0;
   } catch(const app::ParseException& e) {
      ERROR(utils::fmt_to_str("%s\n"
         "Try '%s -?' or '%s --help' instead for more information.", e.what(), PROJECT_NAME, PROJECT_NAME));
   } catch(const std::runtime_error& e) {
      ERROR(e.what());
   }

   return 1;
}