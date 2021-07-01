// (c) Copyright 2020-2021 HP Development Company, L.P.

#pragma once

#include <filesystem>

namespace HP {
namespace Omnicept {
namespace Utils {
    struct FileSystem
    {
        static std::filesystem::path getExePath();
    };
}
}
}