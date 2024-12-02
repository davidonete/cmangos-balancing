SET @START_SPELL_ID := 33393;
SET @END_SPELL_ID := @START_SPELL_ID + 5;
DELETE FROM `spell_template` WHERE `Id` BETWEEN  @START_SPELL_ID AND @END_SPELL_ID;

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

SET @START_ITEM_ID := 25819;
SET @END_ITEM_ID := @START_ITEM_ID + 3;
DELETE FROM `item_template` WHERE `entry` BETWEEN  @START_ITEM_ID AND @END_ITEM_ID;
DELETE FROM `locales_item` WHERE `entry` BETWEEN  @START_ITEM_ID AND @END_ITEM_ID;
DELETE FROM `creature_loot_template` WHERE `item` BETWEEN  @START_ITEM_ID AND @END_ITEM_ID;