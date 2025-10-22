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

                        case ExtendedSpellEffects::SPELL_EFFECT_HEAL_PCT: 
                        {
                            HandleHealPct(spell, spellEffectIndex, caster, victims);
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

    bool RollSpellCritOutcome(Unit* caster, const Unit* victim, SpellSchoolMask schoolMask, const SpellEntry* spell)
    {
        if (!caster || !caster->CanCrit(schoolMask))
        {
            return false;
        }

        if (spell->DmgClass == SPELL_DAMAGE_CLASS_MELEE && victim->IsPlayer() && !victim->IsStandState())
        {
            return true;
        }

        return roll_chance_combat(caster->CalculateSpellCritChance(victim, schoolMask, spell));
    } 

    void CalculateSpellDamage(SpellNonMeleeDamage* spellDamageInfo, int32 damage, SpellEntry const* spellInfo, SpellEffectIndex effectIndex, WeaponAttackType attackType)
    {
        SpellSchoolMask damageSchoolMask = GetSchoolMask(spellDamageInfo->school);
        Unit* caster = static_cast<Unit*>(spellDamageInfo->attacker);
        Unit* pVictim = spellDamageInfo->target;

        if (damage < 0)
        {
            return;
        }

        if (!pVictim)
        {
            return;
        }

        // Check spellInfo crit chance
        bool crit = RollSpellCritOutcome(caster, pVictim, damageSchoolMask, spellInfo);

        // damage bonus (per damage class)
        switch (spellInfo->DmgClass)
        {
            // Melee and Ranged Spells
            case SPELL_DAMAGE_CLASS_RANGED:
            case SPELL_DAMAGE_CLASS_MELEE:
            {
                // Calculate damage bonus
                damage = caster->MeleeDamageBonusDone(pVictim, damage, attackType, damageSchoolMask, spellInfo, effectIndex, SPELL_DIRECT_DAMAGE);
                damage = pVictim->MeleeDamageBonusTaken(caster, damage, attackType, damageSchoolMask, spellInfo, effectIndex, SPELL_DIRECT_DAMAGE);
                break;
            }
            
            // Magical Attacks
            case SPELL_DAMAGE_CLASS_NONE:
            case SPELL_DAMAGE_CLASS_MAGIC:
            {
                // Calculate damage bonus
                damage = caster->SpellDamageBonusDone(pVictim, damageSchoolMask, spellInfo, effectIndex, damage, SPELL_DIRECT_DAMAGE);
                damage = pVictim->SpellDamageBonusTaken(caster, damageSchoolMask, spellInfo, effectIndex, damage, SPELL_DIRECT_DAMAGE);
                break;
            }
        }

        // if crit add critical bonus
        if (crit && !spellInfo->HasAttribute(SPELL_ATTR_EX3_IGNORE_CASTER_MODIFIERS))
        {
            spellDamageInfo->HitInfo |= SPELL_HIT_TYPE_CRIT;
            damage = caster->CalculateCritAmount(pVictim, damage, spellInfo);
        }

        // damage mitigation
        if (damage > 0)
        {
            // physical damage => armor
            if (damageSchoolMask & SPELL_SCHOOL_MASK_NORMAL)
            {
                damage = caster->CalcArmorReducedDamage(caster, pVictim, damage);
            }
        }
        else
        {
            damage = 0;
        }

        spellDamageInfo->damage = damage;
    }

    bool BalancingModule::OnPeriodicTick(Aura* aura)
    {
        if (GetConfig()->enabled)
        {
            Unit* caster = aura ? aura->GetCaster() : nullptr;
            if (caster)
            {
                bool canCrit = false;
                const SpellEntry* victimSpellInfo = aura->GetSpellProto();
                for (Aura* casterAura : caster->GetAurasByType(static_cast<AuraType>(SPELL_AURA_EXTENDED)))
                {
                    const SpellEntry* auraSpellInfo = casterAura->GetSpellProto();
                    for (uint8 spellEffectIndex = 0; spellEffectIndex < MAX_EFFECT_INDEX; spellEffectIndex++)
                    {
                        const ExtendedSpellEffects spellEffect = (ExtendedSpellEffects)(auraSpellInfo->EffectMiscValue[spellEffectIndex]);
                        if (spellEffect == ExtendedSpellEffects::SPELL_EFFECT_PERIODIC_CRIT)
                        {
                            if (victimSpellInfo->IsFitToFamily(SpellFamily(auraSpellInfo->SpellFamilyName), auraSpellInfo->SpellFamilyFlags))
                            {
                                canCrit = true;
                                break;
                            }
                        }
                    }
                }

                if (canCrit)
                {
                    HandlePeriodicSpellCrit(aura);
                    aura->OnPeriodicTickEnd();
                    return true;
                }
            }
        }

        return false;
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

    void BalancingModule::HandleHealPct(const Spell* spell, uint8 spellEffectIndex, Unit* caster, const std::vector<Unit*>& victims)
    {
        const int32 pct = spell->m_spellInfo->EffectBasePoints[spellEffectIndex];

        for (Unit* victim : victims)
        {
            const int32 healAmount = victim->GetMaxHealth() * pct / 100;
            caster->DealHeal(victim, healAmount, spell->m_spellInfo);
        }
    }

    void BalancingModule::HandlePeriodicSpellCrit(Aura* aura) 
    {
        Unit* target = aura->GetTarget();
        Unit* caster = aura->GetCaster();

        if (target && target->IsAlive())
        {
            const Modifier* auraModifier = aura->GetModifier();
            const SpellEntry* spellProto = aura->GetSpellProto();

            uint32 absorb = 0;
            int32 resist = 0;

            uint32 amount = (auraModifier->m_amount > 0 ? uint32(auraModifier->m_amount) : 0);

            uint32 pdamage = amount;
            aura->OnPeriodicCalculateAmount(pdamage);

            if (spellProto->Effect[aura->GetEffIndex()] == SPELL_EFFECT_PERSISTENT_AREA_AURA)
            {
                if (!caster)
                {
                    sLog.outCustomLog("Spell ID: %u Caster guid %lu", spellProto->Id, aura->GetCasterGuid().GetRawValue());
                    MANGOS_ASSERT(caster);
                }
                else if (Unit::SpellHitResult(caster, target, spellProto, (1 << aura->GetEffIndex()), false) != SPELL_MISS_NONE)
                {
                    return;
                }
            }

            if (!spellProto->HasAttribute(SPELL_ATTR_NO_IMMUNITIES))
            {
                if (target->IsImmuneToDamage(GetSpellSchoolMask(spellProto)))
                {
                    Unit::SendSpellOrDamageImmune(aura->GetCasterGuid(), target, spellProto->Id);
                    return;
                }
            }

            bool isNotBleed = GetEffectMechanic(spellProto, aura->GetEffIndex()) != MECHANIC_BLEED;
            if ((spellProto->DmgClass == SPELL_DAMAGE_CLASS_NONE && isNotBleed) || spellProto->DmgClass == SPELL_DAMAGE_CLASS_MAGIC)
            {
                pdamage = target->SpellDamageBonusTaken(caster, GetSpellSchoolMask(spellProto), spellProto, aura->GetEffIndex(), pdamage, DOT, aura->GetStackAmount());
            }
            else
            {
                WeaponAttackType attackType = GetWeaponAttackType(spellProto);
                pdamage = target->MeleeDamageBonusTaken(caster, pdamage, attackType, GetSpellSchoolMask(spellProto), spellProto, aura->GetEffIndex(), DOT, aura->GetStackAmount());
            }

            target->CalculateDamageAbsorbAndResist(caster, GetSpellSchoolMask(spellProto), DOT, pdamage, &absorb, &resist, IsReflectableSpell(spellProto),
                                                   IsResistableSpell(spellProto));

            Unit::DealDamageMods(caster, target, pdamage, &absorb, DOT, spellProto);

            // Set trigger flag
            uint32 procAttacker = PROC_FLAG_DEAL_HARMFUL_PERIODIC;
            uint32 procVictim = PROC_FLAG_TAKE_HARMFUL_PERIODIC;

            const uint32 bonus = (resist < 0 ? uint32(std::abs(resist)) : 0);
            pdamage += bonus;
            const uint32 malus = (resist > 0 ? (absorb + uint32(resist)) : absorb);
            pdamage = (pdamage <= malus ? 0 : (pdamage - malus));

            SpellNonMeleeDamage spellDamageInfo(caster, target, spellProto->Id, SpellSchools(spellProto->School));
            CalculateSpellDamage(&spellDamageInfo, pdamage, spellProto, EFFECT_INDEX_0, BASE_ATTACK);
            pdamage = spellDamageInfo.damage;

            SpellPeriodicAuraLogInfo pInfo(aura, pdamage, absorb, resist, 0.0f);
            target->SendSpellNonMeleeDamageLog(&spellDamageInfo);

            if (pdamage > 0)
            {
                procVictim |= PROC_FLAG_TAKE_ANY_DAMAGE;
            }

            CleanDamage cleanDamage(pdamage, BASE_ATTACK, spellDamageInfo.HitInfo & SPELL_HIT_TYPE_CRIT ? MELEE_HIT_CRIT : MELEE_HIT_NORMAL, pdamage || absorb);
            Unit::DealDamage(caster, target, pdamage, &cleanDamage, DOT, GetSpellSchoolMask(spellProto), spellProto, true);

            Unit::ProcDamageAndSpell(ProcSystemArguments(caster, target, procAttacker, procVictim, PROC_EX_NORMAL_HIT, pdamage, absorb, BASE_ATTACK, spellProto));
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