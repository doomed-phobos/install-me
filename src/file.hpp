#pragma once
#include "src/fwd.hpp"

namespace utils {
   class WFile {
   public:
      WFile(const fs::path& filepath);
      ~WFile();

      bool write8(uint8_t value);
      bool write16(uint16_t value);
      bool write32(uint32_t value);
      bool writeText(const char text[]);
      bool write(const void* buffer, size_t size);
   private:
      FILE* m_file;
   };

   class File {
   public:
      File(const fs::path& filepath);
      ~File();

      bool readS8(int8_t* i);
      bool readS16(int16_t* i);
      bool readS32(int32_t* i);
      bool readU8(uint8_t* i) {return readS8((int8_t*)i);}
      bool readU16(uint16_t* i) {return readS16((int16_t*)i);}
      bool readU32(uint32_t* i) {return readS32((int32_t*)i);}
      size_t read(void* buffer, size_t size);

      bool isAtEnd() const;
   private:
      static size_t fgetsize(FILE* f);
      static size_t qread(FILE* f, void* buffer, size_t count, size_t offset);

      size_t m_current;
      size_t m_end;
      FILE* m_file;
   };
} // namespace utils