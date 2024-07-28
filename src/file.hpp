#pragma once
#include "fwd.hpp"
#include "fs.hpp"

#include <memory>

namespace utils {
  namespace priv {
    class File_base {
    public:
      struct Deleter {
        void operator()(FILE* file) const;
      };

      typedef std::unique_ptr<FILE, Deleter> FilePtr;

      File_base() {}
      File_base(const fs::path& filepath, const char* mode);
      void close();

      const fs::path& filepath() const {return m_filepath;}
    protected:
      auto& file(this auto&& self) {return self.m_file;}

      fs::path m_filepath;
    private:
      FilePtr m_file;
    };
  } // namespace priv

  class WFile : public priv::File_base {
  public:
    explicit WFile(const fs::path& filepath);
    WFile() {}

    bool open(const fs::path& filepath);

    bool write8(uint8_t value);
    bool write16(uint16_t value);
    bool write32(uint32_t value);
    bool writePath(const fs::path& path);
    bool writeText(const char text[]);
    bool write(const void* buffer, size_t size);

    void flush();
  };

  class File : public priv::File_base {
  public:
    File() {}
    explicit File(const fs::path& filepath);

    bool open(const fs::path& filepath);

    bool readS8(int8_t* i);
    bool readS16(int16_t* i);
    bool readS32(int32_t* i);
    bool readU8(uint8_t* i) {return readS8((int8_t*)i);}
    bool readU16(uint16_t* i) {return readS16((int16_t*)i);}
    bool readU32(uint32_t* i) {return readS32((int32_t*)i);}
    bool readPath(fs::path* path);
    size_t read(void* buffer, size_t size);

    bool isAtEnd() const;
  private:
    static size_t fgetsize(FILE* f);

    size_t m_current;
    size_t m_end;
  };
} // namespace utils