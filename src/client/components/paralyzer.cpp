#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "game/paralyzer_fix.hpp"

namespace paralyzer
{
    class component final : public component_interface
    {
    public:
        void post_start() override
        {
            // February/September 2026; March uses 0x129A2C7 and 0x1299DEA.
            if (!game::paralyzer_fix::install(OFFSET(0x14129A2E7), OFFSET(0x141299E0A)))
            {
                game::minlog.WriteLine("Unable to install the Paralyzer/Jetgun null heat-percent fix: instruction mismatch or exception-handler registration failed.");
                component_loader::trigger_premature_shutdown();
            }
            game::minlog.WriteLine("Paralyzer/Jetgun null heat-percent fix enabled");
        }

        void pre_destroy() override
        {
            game::paralyzer_fix::uninstall();
        }
    };
}

REGISTER_COMPONENT(paralyzer::component)
