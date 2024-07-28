#pragma once
#include <iostream>
#include <iomanip>
#include <tuple>
#include <string>
#include <vector>

namespace utils {
  namespace priv {
    template<typename T, size_t N, typename... Ts>
    struct tuple_generator : tuple_generator<T, N-1, T, Ts...> {};

    template<typename T, typename... Ts>
    struct tuple_generator<T, 0, Ts...> {
      typedef std::tuple<Ts...> type;
    };

    template<typename T, size_t N>
    using same_ntuple = tuple_generator<T, N>::type;

    struct center {
      const std::string& str;

      friend std::ostream& operator<<(std::ostream&, const center&);
    };

    inline std::ostream& operator<<(std::ostream& out, const center& c) {
      const auto w = out.width();
        if(w > c.str.length()) {
          const auto left = (w + c.str.size()) / 2;
          out.width(left);
          out << c.str;
          out.width(w - left);
          out << "";
        } else {
          out << c.str;
        }
      
      return out;
    }
  } // namespace priv

  template<size_t NCol>
  class table {
  public:
    typedef priv::same_ntuple<std::string, NCol> Row;
    table() :
      table(0) {}
    table(size_t colPadding) :
      m_maxLength{},
      m_colPadding{colPadding} {}

    void setTitle(const std::string& title) {
      m_title = title;
    }

    void addRow(Row row) {
      m_rows.emplace_back(row);
      [&row, this]<size_t... Ns>(std::index_sequence<Ns...>) {
        ((std::get<Ns>(m_maxCols) = std::max(std::get<Ns>(row).size() + m_colPadding*2, std::get<Ns>(m_maxCols))), ...);
        m_maxLength = std::max((std::get<Ns>(m_maxCols) + ...) + NCol + 1, m_maxLength);
      }(std::make_index_sequence<NCol>{});
    }

    void print(std::ostream& out) const {
      auto print_row = [&out, this]<size_t... Ns>(const Row& row, std::index_sequence<Ns...>) {
        out << "|";
        ((out << std::setw(std::get<Ns>(m_maxCols)) << std::setfill(' ') << priv::center(std::get<Ns>(row)) << "|"), ...);
        out << "\n";
      };

      if(!m_title.empty()) {
        out << '+' << std::setfill('-') << std::setw(m_maxLength-1) << "+" << "\n"
            << '|' << std::setfill(' ') << std::setw(m_maxLength-2) << priv::center(m_title) << "|" << "\n";
      }
      
      for(const auto& row : m_rows) {
        out << '+' << std::setfill('-') << std::setw(m_maxLength-1) << "+" << "\n";
        print_row(row, std::make_index_sequence<NCol>{});
      }
      out << '+' << std::setfill('-') << std::setw(m_maxLength-1) << "+" << "\n";
    }
  private:
    std::vector<Row> m_rows;
    std::string m_title;
    priv::same_ntuple<size_t, NCol> m_maxCols;
    size_t m_maxLength;
    size_t m_colPadding;
  };
} // namespace utils