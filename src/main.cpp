#include "app.hpp"
#include "package_manager.hpp"
#include "parse_exception.hpp"
#include "output.hpp"
#include "generate_macros.hpp"

int main(int argc, char* argv[]) {
  try {
    app::App app(argc, argv);
    app.run();

    return 0;
  } catch(const app::ParseException& e) {
    ERROR("{0}\n"
          "Try '{1} -?' or '{1} --help' instead for more information.", e.what(), PROJECT_NAME);
  } catch(const std::runtime_error& e) {
    ERROR(e.what());
  }

  return 1;
}