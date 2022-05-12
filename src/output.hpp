#pragma once
#include <iostream>
#include <iomanip>
#include <vector>

namespace app {
   class Output {
   public:
      static Output* Instance();

      virtual void info(const std::string& msg) = 0;
      virtual void verbose(const std::string& msg) = 0;
      virtual void warning(const std::string& msg) = 0;
      virtual void error(const std::string& msg) = 0;
   private:
      friend class App;

      static void SetInstance(Output* instance);
   };

   /// Only error msg
   class DefaultOutput : public Output {
   public:
      virtual void info(const std::string& msg) override;
      virtual void verbose(const std::string& msg) override;
      virtual void warning(const std::string& msg) override;
      virtual void error(const std::string& msg) override;
   protected:
      static void puts_with_sign(std::ostream& out, char sign, const std::string& msg);
   };

   class VerboseOutput : public DefaultOutput {
   public:
      virtual void verbose(const std::string& msg) override;
   };
} // namespace app

namespace utils {
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
#define ERROR(msg) app::Output::Instance()->error(msg)