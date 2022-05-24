#include "src/file.hpp"

#include "src/fs.hpp"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <unistd.h>

namespace utils {
   namespace priv {
      File_base::File_base(const fs::path& filepath, const char* mode) :
         m_file(std::shared_ptr<FILE>(fopen(filepath.c_str(), mode), close_if_valid)) {
         if(!m_file)
            throw std::runtime_error("Unable to read/write '" + filepath.string() + "'");
      }

      void File_base::close_if_valid(FILE* file) {
         if(file)
            fclose(file);
      }
   } // namespace priv

   WFile::WFile(const fs::path& filepath) :
      priv::File_base(filepath, "wb") {}

   bool WFile::write8(uint8_t value)        {return write(&value, 1);}
   bool WFile::write16(uint16_t value)      {return write(&value, 2);}
   bool WFile::write32(uint32_t value)      {return write(&value, 4);}
   bool WFile::writeText(const char text[]) {return write(text, strlen(text));}
   bool WFile::write(const void* buffer, size_t size) {
      if(!m_file)
         return false;
      
      if(fwrite(buffer, 1, size, m_file.get()) != size) {
         m_file.reset();
         return false;
      }

      return true;
   }

   void WFile::flush() {
      if(m_file)
         fflush(m_file.get());
   }

   File::File(const fs::path& filepath) :
      priv::File_base(filepath, "rb") {
      m_current = 0;
      m_end = fgetsize(m_file.get());
   }

   bool File::readS8(int8_t* i) {
      return read(i, sizeof(*i)) == sizeof(*i);
   }
   bool File::readS16(int16_t* i) {
      return read(i, sizeof(*i)) == sizeof(*i);
   }
   bool File::readS32(int32_t* i) {
      return read(i, sizeof(*i)) == sizeof(*i);
   }

   size_t File::read(void* buffer, size_t size) {
      if(size > m_end - m_current)
         size = m_end - m_current;
      
      size_t bytesRead = size;
      if(buffer)
         bytesRead = qread(m_file.get(), buffer, size, m_current);

      if(bytesRead == SIZE_MAX)
         return 0;
      m_current += bytesRead;
      return bytesRead;
   }

   bool File::isAtEnd() const {
      if(m_current == m_end)
         return true;
      
      return m_current >= fgetsize(m_file.get());
   }

   size_t File::fgetsize(FILE* f) {
      assert(f != nullptr);

      long curr = ftell(f);
      if(curr < 0)
         return 0;

      fseek(f, 0, SEEK_END);
      long size = ftell(f);
      if(size < 0)
         size = 0;
      
      fseek(f, curr, SEEK_SET);
      return size;
   }

   size_t File::qread(FILE* f, void* buffer, size_t count, size_t offset) {
      int fd = fileno(f);
      if(fd < 0)
         return SIZE_MAX;
      
      ssize_t bytesRead = pread(fd, buffer, count, offset);
      if(bytesRead < 0)
         return SIZE_MAX;

      return bytesRead;
   }
} // namespace utils