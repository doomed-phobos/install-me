#pragma once

namespace app {
  class AppFlags {
  public:
    enum Enum : unsigned char {
      kNone            = 0,
      kSymLink         = 1 << 1,
    };

    AppFlags();

    void setFlags(Enum flags);
    void addFlags(Enum flags);

    bool hasFlags(Enum flags) const;
  private:
    unsigned m_flags;
  };
} // namespace app