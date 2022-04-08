#pragma once
#include <cstdint>

class AppFlags {
public:
   enum Enum : uint8_t {
      kNone            = 0,
      kIncludeAllFiles = 1 << 0,
      kForce           = 1 << 1,
      kRecursive       = 1 << 2,
      kVerbose         = 1 << 3
   };

   AppFlags();

   void setFlags(Enum flags);
   void addFlags(Enum flags);

   bool hasFlags(Enum flags) const;
private:
   unsigned m_flags;
};