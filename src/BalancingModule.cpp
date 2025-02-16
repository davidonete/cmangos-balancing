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
                    const uint32 targetMode = spellInfo->EffectImplicitTargetA[spellEffectIndex];
                    std::vector<Unit*> victims = GetImplicitTargets(targetMode, spellInfo, spellEffectIndex, caster, spellTarget);

                    const ExtendedSpellEffects spellEffect = (ExtendedSpellEffects)(spellInfo->EffectMiscValue[spellEffectIndex]);
                    switch (spellEffect)
                    {
                        case ExtendedSpellEffects::SPELL_EFFECT_REFRESH_AURA:
                        {
                            HandleRefreshAura(spell, spellEffectIndex, caster, victims);
                            break;
                        }

                        case ExtendedSpellEffects::SPELL_EFFECT_REFRESH_AURA_FAMILY_MASK:
                        {
                            HandleRefreshAuraByFamilyMask(spell, spellEffectIndex, caster, victims);
                            break;
                        }

                        case ExtendedSpellEffects::SPELL_EFFECT_REMOVE_AURA:
                        {
                            HandleRemoveAura(spell, spellEffectIndex, caster, victims);
                            break;
                        }

                        case ExtendedSpellEffects::SPELL_EFFECT_CLONE_SPELL:
                        {
                            HandleCloneSpell(spell, spellEffectIndex, caster);
                            break;
                        }

                        default: break;
                    }
                }
            }
        }
    }

    void BalancingModule::OnProc(const ProcExecutionData& data, SpellAuraProcResult& procResult)
    {
        if (GetConfig()->enabled)
        {
            const Aura* triggeredByAura = data.triggeredByAura;
            if (triggeredByAura)
            {
                Unit* caster = triggeredByAura->GetCaster();
                const SpellEntry* spellInfo = triggeredByAura->GetSpellProto();
                SpellEffectIndex spellEffectIndex = triggeredByAura->GetEffIndex();

                if (spellInfo->Effect[spellEffectIndex] == SPELL_EFFECT_APPLY_AURA && spellInfo->EffectApplyAuraName[spellEffectIndex] == SPELL_AURA_EXTENDED)
                {
                    const uint32 targetMode = spellInfo->EffectMechanic[spellEffectIndex] != 0 ? (uint32)(spellInfo->EffectMechanic[spellEffectIndex]) : spellInfo->EffectImplicitTargetA[spellEffectIndex];
                    std::vector<Unit*> victims = GetImplicitTargets(targetMode, spellInfo, spellEffectIndex, caster, data.victim);

                    const ExtendedSpellEffects spellEffect = (ExtendedSpellEffects)(spellInfo->EffectMiscValue[spellEffectIndex]);
                    switch (spellEffect)
                    {
                        case ExtendedSpellEffects::SPELL_EFFECT_ENERGIZE_BY_DAMAGE_DONE:
                        {
                            const uint32 damage = data.damage;
                            HandleEnergizeByDamageDone(data.damage, spellInfo, spellEffectIndex, caster, victims);
                            break;
                        }

                        default: break;
                    }
                }
            }
        }
    }

    void BalancingModule::HandleRefreshAura(const Spell* spell, uint8 spellEffectIndex, Unit* caster, const std::vector<Unit*>& victims)
    {
        const uint32 spellID = spell->m_spellInfo->EffectTriggerSpell[spellEffectIndex];

        for (Unit* victim : victims)
        {
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

    void BalancingModule::HandleRefreshAuraByFamilyMask(const Spell* spell, uint8 spellEffectIndex, Unit* caster, const std::vector<Unit*>& victims)
    {
        const ObjectGuid casterGuid = caster ? caster->GetObjectGuid() : ObjectGuid();
        const SpellFamily family = (SpellFamily)(spell->m_spellInfo->SpellFamilyName);
        const uint64 familyMask = spell->m_spellInfo->EffectTriggerSpell[spellEffectIndex];

        for (Unit* victim : victims)
        {
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

    void BalancingModule::HandleRemoveAura(const Spell* spell, uint8 spellEffectIndex, Unit* caster, const std::vector<Unit*>& victims)
    {
        const ObjectGuid casterGuid = caster ? caster->GetObjectGuid() : ObjectGuid();
        const uint32 spellID = spell->m_spellInfo->EffectTriggerSpell[spellEffectIndex];
        const uint32 stackAmount = spell->m_spellInfo->EffectBasePoints[spellEffectIndex];

        for (Unit* victim : victims)
        {
            victim->RemoveAuraHolderFromStack(spellID, stackAmount > 0 ? stackAmount : 9999, casterGuid);
        }
    }

    void BalancingModule::HandleCloneSpell(const Spell* spell, uint8 spellEffectIndex, Unit* caster)
    {
        if (caster)
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

                    // Remove GCD
                    SpellEntry* newSpellEntry = const_cast<SpellEntry*>(sSpellTemplate.LookupEntry<SpellEntry>(originalSpell->m_spellInfo->Id));
                    newSpellEntry->StartRecoveryCategory = 0;
                    newSpellEntry->StartRecoveryTime = 0;

                    uint32 triggeredFlags = TRIGGERED_INSTANT_CAST | TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_COSTS | TRIGGERED_IGNORE_COOLDOWNS;
                    Spell* newSpell = new Spell(caster, newSpellEntry, triggeredFlags, caster->GetObjectGuid(), spell->m_spellInfo);
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

    void BalancingModule::HandleEnergizeByDamageDone(const uint32 damage, const SpellEntry* spellInfo, uint8 spellEffectIndex, Unit* caster, const std::vector<Unit*>& victims)
    {
        const int32 pct = spellInfo->EffectBasePoints[spellEffectIndex];
        const int32 energizeAmount = damage * pct / 100;
        const Powers power = (Powers)(spellInfo->EffectItemType[spellEffectIndex]);

        for (Unit* victim : victims)
        {
            caster->EnergizeBySpell(victim, spellInfo, energizeAmount, power);
        }
    }

    std::vector<Unit*> BalancingModule::GetImplicitTargets(const uint32 targetMode, const SpellEntry* spellInfo, uint8 spellEffectIndex, Unit* caster, Unit* defaultVictim) const
    {
        std::vector<Unit*> targets;
        switch (targetMode)
        {
            case TARGET_UNIT_CASTER:
            {
                if (caster)
                {
                    targets.push_back(caster);
                }

                break;
            }

            case TARGET_ENUM_UNITS_PARTY_WITHIN_CASTER_RANGE:
            {
                if (caster)
                {
                    float radius = 0.0f;
                    if (spellInfo->EffectRadiusIndex[spellEffectIndex])
                    {
                        radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(spellInfo->EffectRadiusIndex[spellEffectIndex]));
                    }
                    else
                    {
                        radius = GetSpellMaxRange(sSpellRangeStore.LookupEntry(spellInfo->rangeIndex));
                    }

                    Player* player = caster->GetBeneficiaryPlayer();
                    Group* group = player ? player->GetGroup() : nullptr;
                    if (group)
                    {
                        uint8 subgroup = player->GetSubGroup();
                        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
                        {
                            Player* target = itr->getSource();
                            if (target && subgroup == target->GetSubGroup() && caster->CanAssistSpell(target, spellInfo))
                            {
                                if (target == caster || caster->IsWithinDistInMap(target, radius))
                                {
                                    targets.push_back(target);
                                }

                                if (Pet* pet = target->GetPet())
                                {
                                    if (pet == caster || caster->IsWithinDistInMap(pet, radius))
                                    {
                                        targets.push_back(pet);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        Unit* ownerOrSelf = player ? player : caster->GetBeneficiary();
                        if (ownerOrSelf == caster || caster->IsWithinDistInMap(ownerOrSelf, radius))
                        {
                            targets.push_back(ownerOrSelf);
                        }

                        if (Pet* pet = ownerOrSelf->GetPet())
                        {
                            if (pet == caster || caster->IsWithinDistInMap(pet, radius))
                            {
                                targets.push_back(pet);
                            }
                        }
                    }
                }

                break;
            }

            default: 
            {
                if (defaultVictim)
                {
                    targets.push_back(defaultVictim);
                }

                break;
            }
        }

        return targets;
    }
}