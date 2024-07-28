#include "output.hpp"

#include <iostream>
#include <ranges>
#include <vector>

namespace app {
  namespace out {
    struct Format {
      enum class Attribute : char {
        Default = 0,
        Bold    = 1,
      } attr = Attribute::Default;

      enum class Foreground : char {
        Default      = 39,
        Black        = 30,
        Red          = 31,
        Green        = 32,
        Yellow       = 33,
        Blue         = 34,
        Magenta      = 35,
        Cyan         = 36,
        LightGray    = 37,
        DarkGray     = 90,
        LightRed     = 91,
        LightGreen   = 92,
        LightYellow  = 93,
        LightBlue    = 94,
        LightMagenta = 95,
        LightCyan    = 96,
        White        = 97
      } fg = Foreground::Default;

    /// "\033[{FORMAT_ATTRIBUTE};{FORGROUND_COLOR};{BACKGROUND_COLOR}m{TEXT}\033[{RESET_FORMATE_ATTRIBUTE}m"
      static std::string format(Format fmt, const std::string& msg) {
        return std::format("\033[{};{};49m{}\033[0m",
          static_cast<int>(fmt.attr), static_cast<int>(fmt.fg), msg);
      }
    };

    void output_with_sign(std::ostream& out, char sign, Format fmt, const std::string& msg) {
      auto lines = msg | std::views::split('\n') | std::ranges::to<std::vector<std::string>>();

      lines.front().insert(0, std::format("[{}] ", sign));
      for(auto& remain : lines | std::views::drop(1))
        remain.insert(0, 4, ' ');

      out << Format::format(fmt, lines | std::views::join_with('\n') | std::ranges::to<std::string>()) << std::endl;
    }

    void info(const std::string& msg) {
      output_with_sign(std::cout, '*', {.fg = Format::Foreground::Green}, msg);
    }

    void verbose(const std::string& msg) {
      if(allow_verbose)
        output_with_sign(std::cout, 'V', {.fg = Format::Foreground::Blue}, msg);
    }

    void warning(const std::string& msg) {
      output_with_sign(std::cout, '!', {.attr = Format::Attribute::Bold, .fg = Format::Foreground::Yellow}, msg);
    }

    void error(const std::string& msg) {
      output_with_sign(std::cerr, 'X', {.attr = Format::Attribute::Bold, .fg = Format::Foreground::Red}, msg);
    }
  } // namespace out
} // namespace app