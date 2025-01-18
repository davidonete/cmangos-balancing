SET @START_SPELL_ID := 33393;
SET @END_SPELL_ID := @START_SPELL_ID + 49;
DELETE FROM `spell_template` WHERE `Id` BETWEEN @START_SPELL_ID AND @END_SPELL_ID;
DELETE FROM `spell_proc_event` WHERE `entry` BETWEEN @START_SPELL_ID AND @END_SPELL_ID;

UPDATE `spell_template` SET `EffectBasePoints1`= -4  WHERE `Id`= 20101;
UPDATE `spell_template` SET `EffectBasePoints1`= -7  WHERE `Id`= 20102;
UPDATE `spell_template` SET `EffectBasePoints1`= -10 WHERE `Id`= 20103;
UPDATE `spell_template` SET `EffectBasePoints1`= -13 WHERE `Id`= 20104;
UPDATE `spell_template` SET `EffectBasePoints1`= -16 WHERE `Id`= 20105;

UPDATE `spell_template` SET `ManaCost`= 150, `EffectBasePoints1`= 29 WHERE `Id`= 20925;
UPDATE `spell_template` SET `ManaCost`= 195, `EffectBasePoints1`= 29 WHERE `Id`= 20927;
UPDATE `spell_template` SET `ManaCost`= 240, `EffectBasePoints1`= 29 WHERE `Id`= 20928;

UPDATE `spell_template` SET `ManaCost`= 135 WHERE `Id`= 26573;
UPDATE `spell_template` SET `ManaCost`= 235 WHERE `Id`= 20116;
UPDATE `spell_template` SET `ManaCost`= 320 WHERE `Id`= 20922;
UPDATE `spell_template` SET `ManaCost`= 435 WHERE `Id`= 20923;
UPDATE `spell_template` SET `ManaCost`= 565 WHERE `Id`= 20924;

UPDATE `spell_template` SET `DurationIndex`= 347 WHERE `Id`= 25898 OR `Id` = 25890 OR `Id` = 25782 OR `Id` = 25916 OR `Id` = 25895 OR `Id` = 25899 OR `Id` = 25894  OR `Id` = 25918;
UPDATE `spell_template` SET `DurationIndex`= 5 WHERE `Id`= 20217 OR `Id` = 19977 OR `Id` = 19978 OR `Id` = 19979 OR `Id` = 19740 OR `Id` = 19834 OR `Id` = 19835  OR `Id` = 19836 OR `Id` = 19837 OR `Id` = 19838 OR `Id` = 1038 OR `Id` = 20911 OR `Id` = 20912 OR `Id` = 20913  OR `Id` = 20914 OR `Id` = 19742 OR `Id` = 19850 OR `Id` = 19852 OR `Id` = 19853 OR `Id` = 19854  OR `Id` = 25290;

UPDATE `spell_template` SET `CastingTimeIndex` = 4 WHERE `Id` = 24239 OR `Id` = 24274 OR `Id` = 24275;

UPDATE `spell_template` SET `SpellFamilyFlags` = 524288 WHERE Id = 20185 OR Id = 20186 OR Id = 20344 OR Id = 20345 OR Id = 20346 OR Id = 20354 OR Id = 20355;
UPDATE `spell_template` SET `SpellFamilyFlags` = 1048576 WHERE Id = 20184;
UPDATE `spell_template` SET `SpellFamilyFlags` = 536870912 WHERE Id = 20188 OR Id = 20300 OR Id = 20301 OR Id = 20302 OR Id = 20303 OR Id = 21183;

UPDATE `spell_template` SET `TargetCreatureType` = 36 WHERE Id = 879 OR Id = 5614 OR Id = 5615 OR Id = 10312 OR Id = 10313 OR Id = 10314;

UPDATE `spell_template` SET `EffectBasePoints1`= -11 WHERE `Id` = 16833;
UPDATE `spell_template` SET `EffectBasePoints1`= -21 WHERE `Id` = 16834;
UPDATE `spell_template` SET `EffectBasePoints1`= -31 WHERE `Id` = 16835;

UPDATE `spell_template` SET `RecoveryTime` = 1000, `ManaCost` = 30 WHERE `Id` = 5217 OR `Id` = 6793 OR `Id` = 9845 OR `Id` = 9846;

UPDATE `spell_template` SET `EffectBasePoints1`=9, `EffectBasePoints2`=9 WHERE `Id`=16998;
UPDATE `spell_template` SET `EffectBasePoints1`=19, `EffectBasePoints2`=19 WHERE `Id`=16999;
UPDATE `spell_template` SET `SpellFamilyFlags`= 8388608 WHERE `Id`= 1079 OR `Id`= 9492 OR `Id`= 9493 OR `Id`= 9752 OR `Id`= 9894 OR `Id`= 9896;
UPDATE `spell_template` SET `SpellFamilyName`= 0, `SpellFamilyFlags`= 0 WHERE `Id`= 9007 OR `Id`= 9824 OR `Id`= 9826;

UPDATE `spell_template` SET `AttributesEx2`=0, `Stances`=0, `StancesNot`=1073741824, `DurationIndex`=29 WHERE `Id`=5570 OR `Id`=24974 OR `Id`=24975 OR `Id`=24976 OR `Id`=24977;

UPDATE `spell_template` SET `EffectBasePoints1`= -3 WHERE `Id`= 16039;
UPDATE `spell_template` SET `EffectBasePoints1`= -5 WHERE `Id`= 16109;
UPDATE `spell_template` SET `EffectBasePoints1`= -7 WHERE `Id`= 16110;
UPDATE `spell_template` SET `EffectBasePoints1`= -9 WHERE `Id`= 16111;
UPDATE `spell_template` SET `EffectBasePoints1`= -11 WHERE `Id`= 16112;

UPDATE `spell_template` SET `EffectBasePoints1`= -3 WHERE `Id`= 19416;
UPDATE `spell_template` SET `EffectBasePoints1`= -5 WHERE `Id`= 19417;
UPDATE `spell_template` SET `EffectBasePoints1`= -7 WHERE `Id`= 19418;
UPDATE `spell_template` SET `EffectBasePoints1`= -9 WHERE `Id`= 19419;
UPDATE `spell_template` SET `EffectBasePoints1`= -11 WHERE `Id`= 19420;

SET @START_ITEM_ID := 25819;
SET @END_ITEM_ID := @START_ITEM_ID + 15;
DELETE FROM `item_template` WHERE `entry` BETWEEN  @START_ITEM_ID AND @END_ITEM_ID;
DELETE FROM `locales_item` WHERE `entry` BETWEEN  @START_ITEM_ID AND @END_ITEM_ID;
DELETE FROM `creature_loot_template` WHERE `item` BETWEEN  @START_ITEM_ID AND @END_ITEM_ID;