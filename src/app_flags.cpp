#include "app_flags.hpp"

namespace app {
   AppFlags::AppFlags() :
      m_flags(0) {}

   void AppFlags::setFlags(Enum flags) {
      m_flags = flags;
   }
   void AppFlags::addFlags(Enum flags) {
      m_flags |= flags;
   }

   bool AppFlags::hasFlags(Enum flags) const {
      return (m_flags & flags) == flags;
   }
} // namespace app