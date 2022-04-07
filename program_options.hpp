#pragma once
#include <string>
#include <vector>
#include <iosfwd>
#include <stdexcept>

class ParseException : public std::runtime_error {
public:
   ParseException(const std::string& msg) :
      std::runtime_error(msg) {}
};

class ProgramOptions {
public:
   class Option {
   public:
      Option(const std::string& name);

      Option& setDescription(const std::string& description);
      Option& setValueName(const std::string& valueName);
      Option& setAliasChr(char aliasChr);
      Option& setRequired(bool required);

      const std::string& description() const;
      const std::string& valueName() const;
      const std::string& name() const;
      char aliasChr() const;
      bool doesRequiresValue() const;
      bool isRequired() const;
   private:
      std::string m_name;
      std::string m_valueName;
      std::string m_description;
      char m_aliasChr;
      bool m_required;
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

   void parse(int argc, char* argv[]);

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