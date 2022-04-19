#pragma once
#include "src/expected.hpp"

#include <string>
#include <vector>
#include <iosfwd>

namespace app {
   class ProgramOptions {
   public:
      class Option {
      public:
         Option(const std::string& name);

         Option& setDescription(const std::string& description);
         Option& setValueName(const std::string& valueName);
         Option& setAliasChr(char aliasChr);

         const std::string& description() const;
         const std::string& valueName() const;
         const std::string& name() const;
         char aliasChr() const;
         bool doesRequiresValue() const;
      private:
         std::string m_name;
         std::string m_valueName;
         std::string m_description;
         char m_aliasChr;
      };

      class Value {
      public:
         Value(const Option* opt, const std::string& value);

         const Option* option() const;
         const std::string& value() const;
      private:
         const Option* m_option;
         std::string m_value;
      };

      typedef std::vector<Option*> OptionList;
      typedef std::vector<Value> ValueList;

      ProgramOptions();
      ~ProgramOptions();

      utils::Expected<void> parse(int argc, char* argv[]);

      Option& add(const std::string& name);

      const OptionList& options() const;
      const ValueList& values() const;
      bool enabled(const Option& opt) const;
      std::string valueOf(const Option& opt) const;
   private:
      OptionList m_options;
      ValueList m_values;
   };

   std::ostream& operator<<(std::ostream& out, const ProgramOptions& po);
} // namespace app