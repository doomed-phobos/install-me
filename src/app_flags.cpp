#include "app_flags.hpp"

namespace app {
  AppFlags::AppFlags() :
    AppFlags(0) {}
  AppFlags::AppFlags(unsigned value) :
    m_value(value) {}

  void AppFlags::setFlags(Enum flags) {
    m_value = flags;
  }
  void AppFlags::addFlags(Enum flags) {
    m_value |= flags;
  }

  bool AppFlags::hasFlags(Enum flags) const {
    return (m_value & flags) == flags;
  }
} // namespace app