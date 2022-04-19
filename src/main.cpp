#include "src/app.hpp"
#include "src/output.hpp"

int main(int argc, char* argv[]) {
   try {
      app::App app(argc, argv);
      app.run();
   } catch(const std::runtime_error& e) {
      ERROR(e.what());
      return 1;
   }

   return 0;
}