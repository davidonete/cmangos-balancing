#pragma once
#include "ModuleConfig.h"

namespace cmangos_module
{
    class BalancingModuleConfig : public ModuleConfig
    {
    public:
        BalancingModuleConfig();
        bool OnLoad() override;

    public:
        bool enabled;
    };
}