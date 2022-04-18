#pragma once
#include <iostream>
#include <iomanip>

namespace app {
   class Output {
   public:
      static Output* Instance();

      virtual void info(const std::string& msg) = 0;
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
      virtual void warning(const std::string& msg) override;
      virtual void error(const std::string& msg) override;
   };

   class VerboseOutput : public DefaultOutput {
   public:
      virtual void info(const std::string& msg) override;
      virtual void warning(const std::string& msg) override;
      virtual void error(const std::string& msg) override;
   };
} // namespace app::out

#define INFO(msg) app::Output::Instance()->info(msg)
#define WARNING(msg) app::Output::Instance()->warning(msg)
#define ERROR(msg) app::Output::Instance()->error(msg)