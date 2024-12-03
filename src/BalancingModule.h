#ifndef CMANGOS_MODULE_BALANCING_H
#define CMANGOS_MODULE_BALANCING_H

#include "Module.h"
#include "BalancingModuleConfig.h"

class Spell;

namespace cmangos_module
{
    class BalancingModule : public Module
    {
    public:
        BalancingModule();
        const BalancingModuleConfig* GetConfig() const override;

    private:
        void OnCast(Spell* spell, Unit* caster, Unit* victim) override;
    
        void HandleRefreshAura(const Spell* spell, uint8 spellEffectIndex, Unit* caster, Unit* victim);
        void HandleRefreshAuraByFamilyMask(const Spell* spell, uint8 spellEffectIndex, Unit* caster, Unit* victim);
    };
}
#endif
