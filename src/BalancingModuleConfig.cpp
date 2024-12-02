#include "BalancingModuleConfig.h"

namespace cmangos_module
{
    BalancingModuleConfig::BalancingModuleConfig()
    : ModuleConfig("balancing.conf")
    , enabled(false)
    {

    }

    bool BalancingModuleConfig::OnLoad()
    {
        enabled = config.GetBoolDefault("Balancing.Enable", false);
        return true;
    }
}