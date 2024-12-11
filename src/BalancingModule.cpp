#include "BalancingModule.h"

#include "Entities/Unit.h"
#include "Spells/Spell.h"
#include "Spells/SpellAuras.h"
#include "Spells/SpellMgr.h"

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
            for (uint8 spellEffectIndex = 0; spellEffectIndex < MAX_EFFECT_INDEX; spellEffectIndex++)
            {
                if (spellInfo->Effect[spellEffectIndex] == SPELL_EFFECT_EXTENDED)
                {
                    Unit* spellTarget = victim ? victim : spell->GetUnitTarget();
                    if (caster && spellInfo->EffectImplicitTargetA[spellEffectIndex] == TARGET_UNIT_CASTER)
                    {
                        spellTarget = caster;
                    }

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

                        case ExtendedSpellEffects::SPELL_EFFECT_REMOVE_AURA:
                        {
                            HandleRemoveAura(spell, spellEffectIndex, caster, spellTarget);
                            break;
                        }

                        case ExtendedSpellEffects::SPELL_EFFECT_CLONE_SPELL:
                        {
                            HandleCloneSpell(spell, spellEffectIndex, caster, spellTarget);
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

    void BalancingModule::HandleRemoveAura(const Spell* spell, uint8 spellEffectIndex, Unit* caster, Unit* victim)
    {
        if (victim)
        {
            const ObjectGuid casterGuid = caster ? caster->GetObjectGuid() : ObjectGuid();
            const uint32 spellID = spell->m_spellInfo->EffectTriggerSpell[spellEffectIndex];
            const uint32 stackAmount = spell->m_spellInfo->EffectBasePoints[spellEffectIndex];
            victim->RemoveAuraHolderFromStack(spellID, stackAmount > 0 ? stackAmount : 9999, casterGuid);
        }
    }

    void BalancingModule::HandleCloneSpell(const Spell* spell, uint8 spellEffectIndex, Unit* caster, Unit* victim)
    {
        if (victim)
        {
            if (caster->IsSpellReady(*spell->m_spellInfo))
            {
                Spell* originalSpell = caster->GetCurrentSpell(CURRENT_MELEE_SPELL);
                if (originalSpell == nullptr)
                {
                    originalSpell = caster->GetCurrentSpell(CURRENT_GENERIC_SPELL);
                }

                if (originalSpell)
                {
                    // Add modifiers on mana cost and casting time to 0
                    // ...

                    if (spell->m_spellInfo->RecoveryTime <= 0)
                    {
                        // Add a small cooldown so the spell doesn't chain trigger
                        caster->AddCooldown(*spell->m_spellInfo, nullptr, false, 1000);
                    }

                    SpellAuraHolder* auraModHolder = nullptr;
                    const uint32 auraModSpellId = spell->m_spellInfo->EffectTriggerSpell[spellEffectIndex];
                    if (const SpellEntry* auraModSpellInfo = sSpellTemplate.LookupEntry<SpellEntry>(auraModSpellId))
                    {
                        auraModHolder = CreateSpellAuraHolder(auraModSpellInfo, caster, caster, nullptr, spell->m_spellInfo);
                        for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
                        {
                            uint8 eff = auraModSpellInfo->Effect[i];
                            if (eff >= MAX_SPELL_EFFECTS)
                            {
                                continue;
                            }

                            if (IsAreaAuraEffect(eff) ||
                                eff == SPELL_EFFECT_APPLY_AURA ||
                                eff == SPELL_EFFECT_PERSISTENT_AREA_AURA)
                            {
                                int32 basePoints = auraModSpellInfo->CalculateSimpleValue(SpellEffectIndex(i));
                                int32 damage = basePoints;
                                Aura* aura = CreateAura(auraModSpellInfo, SpellEffectIndex(i), &damage, &basePoints, auraModHolder, caster);
                                auraModHolder->AddAura(aura, SpellEffectIndex(i));
                            }
                        }
                        
                        if (!caster->AddSpellAuraHolder(auraModHolder))
                        {
                            delete auraModHolder;
                            auraModHolder = nullptr;
                        }
                    }

                    uint32 triggeredFlags = TRIGGERED_INSTANT_CAST | TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_COSTS | TRIGGERED_IGNORE_COOLDOWNS;
                    Spell* newSpell = new Spell(caster, originalSpell->m_spellInfo, triggeredFlags, caster->GetObjectGuid(), spell->m_spellInfo);
                    SpellCastTargets targets;
                    targets.setUnitTarget(originalSpell->GetUnitTarget());
                    newSpell->SpellStart(&targets);

                    if (auraModHolder)
                    {
                        caster->RemoveSpellAuraHolder(auraModHolder);
                    }
                }
            }
        }
    }
}