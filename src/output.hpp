#pragma once
#include <string>
#include <format>

namespace app {
  namespace out {
    void debug(const std::string& msg);
    void info(const std::string& msg);
    void verbose(const std::string& msg);
    void warning(const std::string& msg);
    void error(const std::string& msg);

    template<typename... Args>
    void info(const std::format_string<Args...>& fmt, Args&&... args) {
      info(std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void verbose(const std::format_string<Args...>& fmt, Args&&... args) {
      verbose(std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void warning(const std::format_string<Args...>& fmt, Args&&... args) {
      warning(std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void error(const std::format_string<Args...>& fmt, Args&&... args) {
      error(std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void debug(const std::format_string<Args...>& fmt, Args&&... args) {
      debug(std::format(fmt, std::forward<Args>(args)...));
    }

    static bool allow_verbose{};
  } // namespace out
} // namespace app

#define INFO(...) app::out::info(__VA_ARGS__)
#define DEBUG(...) app::out::debug(__VA_ARGS__)
#define VERBOSE(...) app::out::verbose(__VA_ARGS__)
#define WARNING(...) app::out::warning(__VA_ARGS__)
#define ERROR(...) app::out::error(__VA_ARGS__)

/*namespace utils {
  namespace priv {
    struct center {
        const std::string_view& str;

        friend std::ostream& operator<<(std::ostream&, const center&);
    };
  } // namespace priv

  class table {
  public:
    class column {
    public:
        column(table& parent, const std::string& table, unsigned offset = 1);

        void addItem(const std::string& item);
        void setPadding(std::streamsize padding) {m_padding = padding;}

        const std::string& title() const {return m_title;}
        std::streamsize padding() const {return m_padding;}
        std::streamsize preferredWidth() const;
        std::string getItem(size_t i) const;
    private:
        friend table;

        table& m_parent;
        std::vector<std::string> m_items;
        std::string m_title;
        std::streamsize m_padding;
        size_t m_width;
    };

    table(const std::string& title);

    void addColumn(column&& col);
  private:
    friend std::ostream& operator<<(std::ostream& out, const table& t);

    std::string m_title;
    std::vector<column> m_columns;
    std::stringstream m_ss;
    size_t m_totalRows;
    size_t m_totalWidth;
  };

  inline priv::center center(const std::string_view& str) {return {str};}
} // namespace utils

#define INFO(msg) app::Output::Instance()->info(msg)
#define VERBOSE(msg) app::Output::Instance()->verbose(msg)
#define WARNING(msg) app::Output::Instance()->warning(msg)
#define ERROR(msg) app::Output::Instance()->error(msg)*/