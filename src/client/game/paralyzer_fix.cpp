#include <std_include.hpp>
#include "paralyzer_fix.hpp"

namespace game::paralyzer_fix
{
    namespace
    {
        std::atomic<std::uintptr_t> installed_address{};
        std::atomic<std::uintptr_t> installed_jetgun_address{};
        PVOID handler_handle{};

        bool matches_instruction(const std::uintptr_t address, const bool jetgun)
        {
            // Include the following movss xmm4 instruction prefix. Keep its
            // constants/register setup on both the normal and recovery paths.
            constexpr unsigned char expected[] = {0xF3, 0x41, 0x0F, 0x10, 0x49, 0x1C, 0xF3, 0x0F, 0x10, 0x25};
            // movss xmm0,[r9+1Ch]; movzx r8d,word ptr [rsp+30h]
            constexpr unsigned char jetgun_expected[] = {0xF3, 0x41, 0x0F, 0x10, 0x41, 0x1C, 0x44, 0x0F, 0xB7, 0x44, 0x24, 0x30};
            __try
            {
                return address && std::memcmp(reinterpret_cast<const void*>(address),
                    jetgun ? jetgun_expected : expected, jetgun ? sizeof(jetgun_expected) : sizeof(expected)) == 0;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return false;
            }
        }

        LONG CALLBACK exception_handler(EXCEPTION_POINTERS* exception)
        {
            return recover(exception, installed_address.load(std::memory_order_acquire),
                installed_jetgun_address.load(std::memory_order_acquire));
        }
    }

    LONG recover(EXCEPTION_POINTERS* exception, const std::uintptr_t paralyzer_address, const std::uintptr_t jetgun_address)
    {
        if (!exception || !exception->ExceptionRecord || !exception->ContextRecord)
            return EXCEPTION_CONTINUE_SEARCH;

        const auto& record = *exception->ExceptionRecord;
        auto& context = *exception->ContextRecord;
        const auto fault_address = reinterpret_cast<std::uintptr_t>(record.ExceptionAddress);
        if (!fault_address || (fault_address != paralyzer_address && fault_address != jetgun_address))
            return EXCEPTION_CONTINUE_SEARCH;
        if (record.ExceptionCode != EXCEPTION_ACCESS_VIOLATION ||
            (record.ExceptionFlags & EXCEPTION_NONCONTINUABLE) || record.NumberParameters < 2 ||
            context.Rip != fault_address || context.R9 != 0 ||
            record.ExceptionInformation[0] != 0 || record.ExceptionInformation[1] != 0x1C)
            return EXCEPTION_CONTINUE_SEARCH;

        if (fault_address == jetgun_address)
            context.Xmm0 = {};
        else
            context.Xmm1 = {};
        context.Rip = fault_address + instruction_size;
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    bool install(const std::uintptr_t fault_address, const std::uintptr_t jetgun_address)
    {
        if (handler_handle)
            return installed_address.load(std::memory_order_acquire) == fault_address &&
                installed_jetgun_address.load(std::memory_order_acquire) == jetgun_address;
        if (!matches_instruction(fault_address, false) || !matches_instruction(jetgun_address, true))
            return false;

        installed_address.store(fault_address, std::memory_order_release);
        installed_jetgun_address.store(jetgun_address, std::memory_order_release);
        handler_handle = AddVectoredExceptionHandler(1, exception_handler);
        if (!handler_handle)
        {
            installed_address.store(0, std::memory_order_release);
            installed_jetgun_address.store(0, std::memory_order_release);
        }
        return handler_handle != nullptr;
    }

    bool uninstall()
    {
        if (!handler_handle)
            return true;
        if (!RemoveVectoredExceptionHandler(handler_handle))
            return false;
        handler_handle = nullptr;
        installed_address.store(0, std::memory_order_release);
        installed_jetgun_address.store(0, std::memory_order_release);
        return true;
    }
}
