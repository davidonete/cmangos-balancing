#pragma once
#include "ModuleConfig.h"

namespace cmangos_module
{
    #define SPELL_EFFECT_EXTENDED 122

    /*
    To make the extended spell effects work the spell must be configured with the following parameters
    - Effect(1/2/3) must have SPELL_EFFECT_EXTENDED value
    - EffectMiscValue(1/2/3) must have one of the ExtendedSpellEffects values
    - EffectBasePoints(1/2/3), EffectApplyAuraName(1/2/3), EffectItemType(1/2/3) and EffectTriggerSpell(1/2/3) can be used to store data
    */
    enum class ExtendedSpellEffects : uint8
    {
        SPELL_EFFECT_REFRESH_AURA             = 1,  // The spell will refresh the specified aura if it exists on the implicit target
                                                    // - EffectTriggerSpell(1/2/3): The spell id aura to refresh

        SPELL_EFFECT_REFRESH_AURA_FAMILY_MASK = 2,  // The spell will refresh the specified aura if it exists on the implicit target
                                                    // - SpellFamilyName: The spell family name of the auras to refresh
                                                    // - EffectTriggerSpell(1/2/3): The spell family mask of the auras to refresh

        SPELL_EFFECT_REMOVE_AURA              = 3,  // The spell will remove the aura specified from the implicit target
                                                    // - EffectTriggerSpell(1/2/3): The spell id aura to remove
                                                    // - EffectBasePoints(1/2/3): The amount of stacks to remove (leave to 0 to remove all)

        SPELL_EFFECT_CLONE_SPELL              = 4,  // The spell will clone the triggering spell on it's target
                                                    // - EffectBasePoints(1/2/3): How effective is the cloned spell in percentage (100 being the exact amount from the original)
                                                    // - EffectTriggerSpell(1/2/3): The spell id aura modifier to add for this cast (e.g. increase damage mod)

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