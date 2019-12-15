#pragma once

#include <filesystem>

namespace NConfig {
    class CacheConfig {
    public:
        static const std::filesystem::path CACHE_DIRECTORY;
        static const std::filesystem::path CONVERGENCE_DIRECTORY;
    };
}
