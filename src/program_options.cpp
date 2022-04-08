#include "src/program_options.hpp"
#include "src/string.hpp"

#include <algorithm>
#include <iomanip>

typedef ProgramOptions PO;

class MissingValueOption : public ParseException {
public:
   MissingValueOption(const std::string& optName, const std::string& optValueName) :
      ParseException(utils::fmt_to_str("Missing value in '--%s=<%s>'", optName.c_str(), optValueName.c_str())) {}
};

class MisingAliasValueOption : public ParseException {
public:
   MisingAliasValueOption(char aliasChr, const std::string& optValueName) :
      ParseException(utils::fmt_to_str("Missing value in '-%c <%s>'", aliasChr, optValueName.c_str())) {}
};

class InvalidArgument : public ParseException {
public:
   InvalidArgument(const std::string& arg) :
      ParseException(utils::fmt_to_str("WTF, what is '%s'?", arg.c_str())) {}
};

PO::Option::Option(const std::string& name) :
   m_name(name),
   m_aliasChr(EOF) {}

PO::Option& PO::Option::setDescription(const std::string& description) {m_description = description; return *this;}
PO::Option& PO::Option::setValueName(const std::string& valueName) {m_valueName = valueName; return *this;}
PO::Option& PO::Option::setAliasChr(char aliasChr) {m_aliasChr = aliasChr; return *this;}

const std::string& PO::Option::description() const {return m_description;}
const std::string& PO::Option::valueName() const {return m_valueName;}
const std::string& PO::Option::name() const {return m_name;}
char PO::Option::aliasChr() const {return m_aliasChr;}
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
            if(opt->doesRequiresValue()) {
               if(i+1 >= argc)
                  throw MisingAliasValueOption(opt->aliasChr(), opt->valueName());

               optValue = argv[++i];
            }

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
   size_t max_width = 0;
   for(const auto& opt : po.options()) {
      max_width = std::max(6 + 2 + opt->name().size() +
         (opt->doesRequiresValue() ? 2 + opt->valueName().size() + 1 : 0), max_width);
   }

   for(const auto& opt : po.options()) {
      size_t opt_width = 6 + 2 + opt->name().size() +
         (opt->doesRequiresValue() ? 2 + opt->valueName().size() + 1 : 0);

      if(opt->aliasChr() != EOF) {
         out << std::setw(3) << "-" << opt->aliasChr() << ", ";
      } else {
         out << std::setw(6) << " ";
      }

      out << "--" << opt->name();

      if(opt->doesRequiresValue()) {
         out << "=<" << opt->valueName() << ">";
      }
      
      if(!opt->description().empty()) {
         bool isMultiline = opt->description().find('\n') != std::string::npos;

         if(!isMultiline) {
            out << std::setw(max_width - opt_width + 12/*offset*/) << " " << opt->description();
         } else {
            std::istringstream ss(opt->description());
            std::string line;
            if(std::getline(ss, line)) {
               out << std::setw(max_width - opt_width + 12/*offset*/) << " " << line << "\n";
               while(std::getline(ss, line))
                  out << std::setw(max_width + 12/*offset*/) << " " << line << "\n";
            }
         }
      }

      out << std::endl;
   }

   return out;
}