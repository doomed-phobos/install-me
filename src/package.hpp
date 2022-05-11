#pragma once
#include "src/fs.hpp"

#include <vector>
#include <memory>

namespace app {
   class Package {
   public:
      typedef std::vector<fs::path> PathList;

      Package(const fs::path& in, const fs::path& out, bool symLink);

      void copyFrom(const fs::path& path);

      const std::string& pkgName() const;
      const fs::path& outDir() const;
      const PathList& relativePaths() const;
   private:
      friend class PackageManager;
      
      Package(const fs::path& out, const std::string& pkgName);
      void addPath(const fs::path& path);
   private:
      bool m_symLink;
      fs::path m_inDir;
      fs::path m_outDir;
      PathList m_paths;
      std::string m_pkgName;
   };
} // namespace app