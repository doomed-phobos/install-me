#include "src/program_options.hpp"
#include "src/string.hpp"

#include <algorithm>
#include <iomanip>
#include <cmath>

namespace {
   class center {
   public:
      center(const std::string_view& str) :
         m_str(str) {}
   private:
      friend std::ostream& operator<<(std::ostream& out, const center& c);

      const std::string_view& m_str;
   };

   std::ostream& operator<<(std::ostream& out, const center& c) {
      std::streamsize w = out.width();
      if(w > c.m_str.length()) {
         std::streamsize left = (w + c.m_str.length()) / 2;
         out.width(left);
         out << c.m_str;
         out.width(w - left);
         out << "";
      } else {
         out << c.m_str;
      }

      return out;
   }
}

namespace app {
   typedef ProgramOptions PO;

   class MissingValueOption : public std::runtime_error {
   public:
      MissingValueOption(const std::string& optName, const std::string& optValueName) :
         std::runtime_error(utils::fmt_to_str("Missing value in '--%s=<%s>'", optName.c_str(), optValueName.c_str())) {}
   };

   class MisingAliasValueOption : public std::runtime_error {
   public:
      MisingAliasValueOption(char aliasChr, const std::string& optValueName) :
         std::runtime_error(utils::fmt_to_str("Missing value in '-%c <%s>'", aliasChr, optValueName.c_str())) {}
   };

   class InvalidArgument : public std::runtime_error {
   public:
      InvalidArgument(const std::string& arg) :
         std::runtime_error(utils::fmt_to_str("WTF, what is '%s'?", arg.c_str())) {}
   };

   class InvalidUniqueOption : public std::runtime_error {
   public:
      InvalidUniqueOption(const std::string& arg) :
         std::runtime_error(utils::fmt_to_str("Option '%s' should be the only", arg.c_str())) {}
   };

   PO::Option::Option(const std::string& name) :
      m_name(name),
      m_aliasChr(EOF),
      m_unique(false) {}

   PO::Option& PO::Option::setDescription(const std::string& description) {m_description = description; return *this;}
   PO::Option& PO::Option::setValueName(const std::string& valueName) {m_valueName = valueName; return *this;}
   PO::Option& PO::Option::setAliasChr(char aliasChr) {m_aliasChr = aliasChr; return *this;}
   PO::Option& PO::Option::setUniqueOption(bool unique) {m_unique = unique; return *this;}

   const std::string& PO::Option::description() const {return m_description;}
   const std::string& PO::Option::valueName() const {return m_valueName;}
   const std::string& PO::Option::name() const {return m_name;}
   char PO::Option::aliasChr() const {return m_aliasChr;}
   bool PO::Option::isUniqueOption() const {return m_unique;}
   bool PO::Option::doesRequiresValue() const {return !m_valueName.empty();}

   PO::Value::Value(const Option* opt, const std::string& value) :
      m_option(opt),
      m_value(value) {}

   const PO::Option* PO::Value::option() const {return m_option;}
   const std::string& PO::Value::value() const {return m_value;}

   PO::ProgramOptions() {}
   PO::~ProgramOptions() {
      for(auto& opt : m_options)
         delete opt;
      
      m_options.clear();
      m_values.clear();
   }

   void PO::parse(int argc, char* argv[]) {
      for(int i = 1; i < argc; ++i) {
         std::string arg(argv[i]);

         int n_dashes = 0;
         for(; arg[n_dashes] == '-'; ++n_dashes);

         std::string optName;
         std::string optValue;

         if(n_dashes == 2) {
            size_t equalSignPos = arg.find('=', n_dashes);
            
            if(equalSignPos != std::string::npos) {
               optName = arg.substr(n_dashes, equalSignPos-n_dashes);
               optValue = arg.substr(equalSignPos+1);
            } else {
               optName = arg.substr(n_dashes);
            }

            OptionList::iterator it = std::find_if(
               m_options.begin(),
               m_options.end(),
               [&](const Option* opt) -> bool {
                  return opt->name() == optName;
               }
            );

            if(it != m_options.end()) {
               Option* opt = *it;
               
               if(opt->doesRequiresValue())
                  if(equalSignPos == std::string::npos)
                     throw MissingValueOption(optName, opt->valueName());

               if(opt->isUniqueOption() && argc-1 != 1)
                  throw InvalidUniqueOption(arg);

               m_values.push_back(Value(opt, optValue));
            } else {
               optName.clear();
            }
         } else if(n_dashes == 1) {
            optName = arg.substr(n_dashes);

            OptionList::iterator it = std::find_if(
               m_options.begin(),
               m_options.end(),
               [&](const Option* opt) -> bool {
                  return opt->aliasChr() == optName[0];
               }
            );

            if(it != m_options.end()) {
               Option* opt = *it;
               int max = 1;
               if(opt->doesRequiresValue()) {
                  if(i+1 >= argc)
                     throw MisingAliasValueOption(opt->aliasChr(), opt->valueName());

                  optValue = argv[++i];
                  ++max;
               }

               if(opt->isUniqueOption() && argc-1 != max)
                  throw InvalidUniqueOption(arg);

               m_values.push_back(Value(opt, optValue));
            } else {
               optName.clear();   
            }
         }

         if(optName.empty())
            throw InvalidArgument(arg);
      }
   }

   PO::Option& PO::add(const std::string& name) {
      Option* opt = new Option(name);
      m_options.push_back(opt);
      return *opt;
   }

   const PO::OptionList& PO::options() const {return m_options;}
   const PO::ValueList& PO::values() const {return m_values;}

   bool PO::enabled(const Option& opt) const {
      for(const auto& value : m_values)
         if(value.option() == &opt)
            return true;
      
      return false;
   }

   std::string PO::valueOf(const Option& opt) const {
      for(const auto& value : m_values)
         if(value.option() == &opt)
            return value.value();

      return "";   
   }

   std::ostream& operator<<(std::ostream& out, const ProgramOptions& po) {
      static constexpr const std::string_view title_str = "OPTIONS";
      static constexpr const std::string_view cols_str[] = {
         "Unique", "Short version", "Large version", "Description"
      };
      static constexpr const unsigned UNIQUE        = 0;
      static constexpr const unsigned SHORT_VERSION = 1;
      static constexpr const unsigned LARGE_VERSION = 2;
      static constexpr const unsigned DESCRIPTION   = 3;

      size_t max_width_lv = 0;
      size_t max_width_sv = 0;
      size_t max_width_description = 0;

      for(const auto& opt : po.options()) {
         max_width_lv = std::max(1 + 2 + opt->name().size() + 
            (opt->doesRequiresValue() ? 2 + opt->valueName().size() + 1 : 0) + 1, max_width_lv);
         max_width_sv = std::max(1 + 2 + 
            (opt->doesRequiresValue() ? 2 + opt->valueName().size() + 1 : 0) + 1, max_width_sv);
         max_width_description = std::max(1 + opt->description().size() + 1, max_width_description);
      }

      size_t total_width = 2 + cols_str[UNIQUE].size() + 2 + cols_str[SHORT_VERSION].size() + 1 + max_width_lv + 1 + max_width_description + 2;

      out << std::setfill('-') << "\n" << std::setw(total_width+1) << "";
      out << "\n| " << std::setfill(' ') << std::setw(total_width-3) << center(title_str) << " |";
      out << std::setfill('-') << "\n" << std::setw(total_width+1) << "";

      out << std::setfill(' ') << "\n| " << cols_str[UNIQUE] << " |" <<
             std::setw(max_width_sv) << center(cols_str[SHORT_VERSION]) << "|" << 
             std::setw(max_width_lv) << center(cols_str[LARGE_VERSION]) << "|" << 
             std::setw(max_width_description) << center(cols_str[DESCRIPTION]) << "|\n";
      
      out << std::setfill('-') << std::setw(total_width+1) << "" << "\n";

      out << std::setfill(' ');
      for(const auto& opt : po.options()) {
         out << std::setw(cols_str[UNIQUE].size()+3);
         if(opt->isUniqueOption())
            out << center("*");
         else
            out << "";

         out << std::setw(max_width_sv+2);
         if(opt->aliasChr() != EOF) {
            std::string sv = "-";
            sv.push_back(opt->aliasChr());
            if(opt->doesRequiresValue())
               sv += " <" + opt->valueName() + ">";

            out << center(sv);
         } else {
            out << "";
         }

         std::string lv = "--";
         lv += opt->name();
         if(opt->doesRequiresValue())
            lv += "=<" + opt->valueName() + ">";

         out << std::setw(max_width_lv+1) << center(lv)
             << std::setw(max_width_description) << center(opt->description()) << "\n";
      }

      return out;
   }
} // namespace app