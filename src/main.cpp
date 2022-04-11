#include "src/app.hpp"
#include "src/output.hpp"

int main(int argc, char* argv[]) {
   App app(argc, argv);

   try {
      app.run();
   } catch(const AppException& e) {
      out::error(e.what());
   }

   return 0;
}