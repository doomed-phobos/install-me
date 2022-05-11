#include "src/package.hpp"

#include "src/fs.hpp"
#include "src/output.hpp"

namespace app {
   Package::Package(const fs::path& out, const std::string& pkgName) :
      m_outDir(out),
      m_pkgName(pkgName) {}

   Package::Package(const fs::path& in, const fs::path& out, bool symLink) :
      m_inDir(in),
      m_outDir(out),
      m_pkgName(in.stem().string()),
      m_symLink(symLink) {
      std::error_code e;
      if(fs::create_directory(out, e))
         VERBOSE("Output dir " + m_outDir.string() + " created ...");
      if(e)
         throw std::runtime_error("Output dir: " + e.message());
   }

   void Package::copyFrom(const fs::path& path) {
      if(fs::is_directory(path))
         return;
      
      auto relativePath = fs::relative(path, m_inDir);
      if(relativePath.has_parent_path()) {
         auto newDir = m_outDir/relativePath.parent_path();
         std::error_code e;
         fs::create_directories(newDir, e);
         if(e)
            throw std::runtime_error("Directory creation '" + newDir.string() + "': " + e.message());
         VERBOSE("Directory '" + newDir.string() + "' created ...");
      }
      std::error_code e;
      auto outPath = m_outDir/relativePath;
      fs::copy(path,
         outPath,
         m_symLink ? fs::copy_options::create_symlinks : fs::copy_options::none,
         e);
      if(e)
         throw std::runtime_error("Package copying '" + outPath.string() + "': " + e.message());

      addPath(relativePath);
   }

   void Package::addPath(const fs::path& path) {
      m_paths.push_back(path);
   }

   const std::string& Package::pkgName() const {return m_pkgName;}
   const fs::path& Package::outDir() const {return m_outDir;}
   const Package::PathList& Package::relativePaths() const {return m_paths;}
} // namespace app