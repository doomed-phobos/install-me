#pragma once
#include "src/expected.hpp"
#include "src/fs.hpp"

#include <memory>

namespace app {
   // Located in $HOME/.cache/$PROGRAM
   class CacheManager {
   public:
      /// Return nullptr if any error occurs
      static utils::Expected<void> Initialize();

      static CacheManager* GetInstance();
   private:
      CacheManager(const fs::path& dir);

      static inline std::shared_ptr<CacheManager> instance = nullptr;
      static inline constexpr const char* cache_mgr_dirname = "packages";
   };
} // namespace app