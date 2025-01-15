#ifndef CMANGOS_MODULE_BALANCING_H
#define CMANGOS_MODULE_BALANCING_H

#include "Module.h"
#include "BalancingModuleConfig.h"

class Spell;

struct ProcExecutionData;

namespace cmangos_module
{
    class BalancingModule : public Module
    {
    public:
        BalancingModule();
        const BalancingModuleConfig* GetConfig() const override;

    private:
        void OnCast(Spell* spell, Unit* caster, Unit* victim) override;
        void OnProc(const ProcExecutionData& data, SpellAuraProcResult& procResult) override;
    
        void HandleRefreshAura(const Spell* spell, uint8 spellEffectIndex, Unit* caster, const std::vector<Unit*>& victims);
        void HandleRefreshAuraByFamilyMask(const Spell* spell, uint8 spellEffectIndex, Unit* caster, const std::vector<Unit*>& victims);
        void HandleRemoveAura(const Spell* spell, uint8 spellEffectIndex, Unit* caster, const std::vector<Unit*>& victims);
        void HandleCloneSpell(const Spell* spell, uint8 spellEffectIndex, Unit* caster);
        void HandleEnergizeByDamageDone(const uint32 damage, const SpellEntry* spellInfo, uint8 spellEffectIndex, Unit* caster, const std::vector<Unit*>& victims);
    
        std::vector<Unit*> GetImplicitTargets(const uint32 targetMode, const SpellEntry* spellInfo, uint8 spellEffectIndex, Unit* caster, Unit* defaultVictim) const;
    };
}
#endif
