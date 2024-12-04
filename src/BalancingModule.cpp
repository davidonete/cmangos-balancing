#include "BalancingModule.h"

#include "Entities/Unit.h"
#include "Spells/Spell.h"
#include "Spells/SpellAuras.h"

namespace cmangos_module
{
    BalancingModule::BalancingModule()
    : Module("Balancing", new BalancingModuleConfig())
    {

    }

    const BalancingModuleConfig* BalancingModule::GetConfig() const
    {
        return (BalancingModuleConfig*)Module::GetConfig();
    }

    void BalancingModule::OnCast(Spell* spell, Unit* caster, Unit* victim)
    {
        if (GetConfig()->enabled && spell)
        {
            const SpellEntry* spellInfo = spell->m_spellInfo;
            Unit* spellTarget = victim ? victim : spell->GetUnitTarget();

            for (uint8 spellEffectIndex = 0; spellEffectIndex < MAX_SPELL_EFFECTS; spellEffectIndex++)
            {
                if (spellInfo->Effect[spellEffectIndex] == SPELL_EFFECT_EXTENDED)
                {
                    const ExtendedSpellEffects spellEffect = (ExtendedSpellEffects)(spellInfo->EffectMiscValue[spellEffectIndex]);
                    switch (spellEffect)
                    {
                        case ExtendedSpellEffects::SPELL_EFFECT_REFRESH_AURA:
                        {
                            HandleRefreshAura(spell, spellEffectIndex, caster, spellTarget);
                            break;
                        }

                        case ExtendedSpellEffects::SPELL_EFFECT_REFRESH_AURA_FAMILY_MASK:
                        {
                            HandleRefreshAuraByFamilyMask(spell, spellEffectIndex, caster, spellTarget);
                            break;
                        }

                        default: break;
                    }
                }
            }
        }
    }

    void BalancingModule::HandleRefreshAura(const Spell* spell, uint8 spellEffectIndex, Unit* caster, Unit* victim)
    {
        if (victim)
        {
            const uint32 spellID = spell->m_spellInfo->EffectTriggerSpell[spellEffectIndex];
            for (uint8 spellEffectIndex = 0; spellEffectIndex < MAX_EFFECT_INDEX; spellEffectIndex++)
            {
                if (Aura* aura = victim->GetAura(spellID, (SpellEffectIndex)(spellEffectIndex)))
                {
                    if (caster && aura->GetCasterGuid() != caster->GetObjectGuid())
                    {
                        continue;
                    }

                    if (SpellAuraHolder* holder = aura->GetHolder())
                    {
                        holder->RefreshHolder();
                    }
                }
            }
        }
    }

    void BalancingModule::HandleRefreshAuraByFamilyMask(const Spell* spell, uint8 spellEffectIndex, Unit* caster, Unit* victim)
    {
        if (victim)
        {
            const ObjectGuid casterGuid = caster ? caster->GetObjectGuid() : ObjectGuid();
            const SpellFamily family = (SpellFamily)(spell->m_spellInfo->SpellFamilyName);
            const uint64 familyMask = spell->m_spellInfo->EffectTriggerSpell[spellEffectIndex];

            for (uint32 auraType = SPELL_AURA_BIND_SIGHT; auraType < TOTAL_AURAS; auraType++)
            {
                if (Aura* aura = victim->GetAura((AuraType)(auraType), family, familyMask, casterGuid))
                {
                    if (SpellAuraHolder* holder = aura->GetHolder())
                    {
                        holder->RefreshHolder();
                    }
                }
            }
        }
    }
}