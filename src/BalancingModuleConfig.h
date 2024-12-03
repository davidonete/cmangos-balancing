#pragma once
#include "ModuleConfig.h"

namespace cmangos_module
{
    #define SPELL_EFFECT_EXTENDED 122

    enum class ExtendedSpellEffects : uint8
    {
        SPELL_EFFECT_REFRESH_AURA = 1,
        SPELL_EFFECT_REFRESH_AURA_FAMILY_MASK = 2,
        MAX_SPELL_EFFECTS
    };

    class BalancingModuleConfig : public ModuleConfig
    {
    public:
        BalancingModuleConfig();
        bool OnLoad() override;

    public:
        bool enabled;
    };
}