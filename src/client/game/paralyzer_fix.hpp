#pragma once
#include <Windows.h>
#include <cstdint>

namespace game::paralyzer_fix
{
    // movss xmm1 (Paralyzer) / xmm0 (Jetgun), dword ptr [r9+1Ch]
    inline constexpr unsigned instruction_size = 6;

    LONG recover(EXCEPTION_POINTERS* exception, std::uintptr_t paralyzer_address, std::uintptr_t jetgun_address);
    // Registration/removal are serialized by the component lifecycle.
    bool install(std::uintptr_t paralyzer_address, std::uintptr_t jetgun_address);
    bool uninstall();
}
