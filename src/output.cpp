#include "src/output.hpp"

#include <cassert>
#include <memory>

namespace app {
   static std::shared_ptr<Output> g_instance(new DefaultOutput());

   Output* Output::Instance() {
      return g_instance.get();
   }

   void Output::SetInstance(Output* instance) {
      assert(instance);
      g_instance.reset(instance);
   }

   void DefaultOutput::puts_with_sign(std::ostream& out, char sign, const std::string& msg) {
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

   void DefaultOutput::info(const std::string& msg) {
      puts_with_sign(std::cout, '*', msg);
   }
   void DefaultOutput::verbose(const std::string& msg) {}
   void DefaultOutput::warning(const std::string& msg) {
      puts_with_sign(std::cerr, '!', msg);
   }
   void DefaultOutput::error(const std::string& msg) {
      if(!msg.empty())
         puts_with_sign(std::cerr, 'x', msg + "\nAborting...");
   }

   void VerboseOutput::verbose(const std::string& msg) {
      puts_with_sign(std::cout, 'V', msg);
   }
} // namespace app

namespace utils {
   namespace priv {
      std::ostream& operator<<(std::ostream& out, const center& c) {
         std::streamsize w = out.width();
         if(w > c.str.length()) {
            std::streamsize left = (w + c.str.length()) / 2;
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
   
   // =================
   // Column
   // =================
   table::column::column(table& parent, const std::string& title, unsigned padding) :
      m_parent(parent),
      m_title(title),
      m_padding(padding),
      m_width(m_title.size()) {}
   
   void table::column::addItem(const std::string& item) {
      m_items.push_back(item);
      m_width = std::max<std::streamsize>(item.size(), m_width);
      m_parent.m_totalRows = std::max(m_items.size(), m_parent.m_totalRows);
   }

   std::streamsize table::column::preferredWidth() const {
      return m_width + padding()*2;
   }

   std::string table::column::getItem(size_t i) const {
      if(i >= 0 && i < m_items.size())
         return m_items[i];
      return "";
   }

   // =================
   // Table
   // =================
   table::table(const std::string& title) :
      m_title(title),
      m_totalRows(0),
      m_totalWidth(1) {
      m_ss << "|";
   }
   
   void table::addColumn(column&& col) {
      m_ss << std::setw(col.preferredWidth()) << center(col.title()) << "|";
      m_totalWidth += 1 + col.preferredWidth();
      m_columns.push_back(std::move(col));
   }

   std::ostream& operator<<(std::ostream& out, const table& t) {
      out << std::setfill('-') << std::setw(t.m_totalWidth) << "" << std::endl;
      out << std::setfill(' ') << "|" << std::setw(t.m_totalWidth-2) << center(t.m_title) << "|\n";
      out << std::setfill('-') << std::setw(t.m_totalWidth) << "" << std::endl;
      out << t.m_ss.str() << std::endl;
      out << std::setfill('-') << std::setw(t.m_totalWidth) << "" << std::endl;
      
      out << std::setfill(' ');
      for(size_t i = 0; i < t.m_totalRows; ++i) {
         out << "|";
         for(const auto& col : t.m_columns) {
            out << std::setw(col.preferredWidth()) << center(col.getItem(i)) << "|";
         }
         out << "\n";
      }
      
      return out << std::setfill('-') << std::setw(t.m_totalWidth) << "" << std::endl;
   }

   /*void table::printItems(std::ostream& out) const {
      out << std::setfill(' ');
      for(size_t i = 0; i < m_totalRows; ++i) {
         for(const auto& col : m_columns) {
            out << " " << std::setw(col->preferredWidth()) << center(col->getItem(i)) << "";
         }
         out << "\n";
      }
   }

   void table::print(std::ostream& out) const {
      std::stringstream ss;
      std::streamsize totalWidth = 1;
      for(const auto& col : m_columns) {
         ss << "|" << std::setw(col->preferredWidth()) << center(col->title());
         totalWidth += 1 + col->preferredWidth();
      }
      ss << "|\n";
      
      out << std::setfill('-') << std::setw(totalWidth) << "" << std::endl;
      out << std::setfill(' ') << "|" << std::setw(totalWidth-2) << center(m_title) << "|\n";
      out << std::setfill('-') << std::setw(totalWidth) << "" << std::endl;
      out << ss.str();
      out << std::setfill('-') << std::setw(totalWidth) << "" << std::endl;

      printItems(out);
   }*/
} // namespace utils