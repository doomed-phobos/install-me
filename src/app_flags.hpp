#pragma once

namespace app {
  class AppFlags {
  public:
    enum Enum : unsigned char {
      kNone            = 0,
      kHidden          = 1 << 1,
    };

    AppFlags();
    AppFlags(unsigned value);

    void setFlags(Enum flags);
    void addFlags(Enum flags);

    bool hasFlags(Enum flags) const;
    unsigned value() const {return m_value;}
  private:
    unsigned m_value;
  };
} // namespace app