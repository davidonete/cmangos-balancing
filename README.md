# Balancing
Module to balance classes a little bit better by adding new spells and modifying existing ones. The added spells will be spread across the world in the form of books that can be looted.

List of changes:
- Spells:
	- Paladin
		- Added Crusader Strike (33393): https://www.wowhead.com/wotlk/spell=35395
		- Added Hand of Reckoning (33395): https://www.wowhead.com/wotlk/spell=62124
		- Added Avenger's Shield (33434): https://www.wowhead.com/classic/spell=407669
		- Modified Benediction talent to increase mana reduction up to 30%
		- Modified Holy Shield spell to increase block chance to 50% and reduce a 50% the mana cost
		- Modified Consecration spell to reduce mana cost 50%
		- Modified all blessing spells to increase duration a 100%
		- Modified Hammer of Wrath spell to remove cast time
		- Modified Judgement spells so different debuffs can be placed on the same target
		- Modified Exorcism so it can be casted on any target
	- Druid
		- Added Starfall (33397): https://www.wowhead.com/classic/spell=439748
		- Added Sunfire (33399): https://www.wowhead.com/classic/spell=414684
		- Added Elune's Fires (33401, 33403): https://www.wowhead.com/classic/spell=414719
		- Added Starsurge (33406): https://www.wowhead.com/classic/spell=417157
		- Added Eclipse (33408, 33418, 33410, 33420): https://www.wowhead.com/classic/spell=408248
		- Modified Natural Shapeshifter talent to increase mana reduction up to 60%
		- Modified Tiger's Fury spell to remove energy cost and add a 30 seconds cooldown
		- Modified Savage Fury talen will also increase bleed damage up to a 30%
		- Modified Insect Swarm so it can now be casted in moonkin form and increase duration to 20 seconds
	- Shaman
		- Added Maelstrom Weapon (33413, 33414): https://www.wowhead.com/classic/spell=408498
		- Added Lava Burst (33423, 33424): https://www.wowhead.com/classic/spell=408490
		- Added Water Shield (33429): https://www.wowhead.com/classic/spell=408510
		- Added Overload: https://www.wowhead.com/classic/spell=408438
		- Modified Convection talent to increase mana reduction up to 30%
	- Hunter
		- Added Aspect of the Viper (33436): https://www.wowhead.com/classic/spell=34074
		- Modified Efficiency talent to increase mana reduction up to 30%

# Available Cores
Classic

# Version
9

# How to install
1. Follow the instructions in https://github.com/davidonete/cmangos-modules?tab=readme-ov-file#how-to-install
2. Enable the `BUILD_MODULE_BALANCING` flag in cmake and run cmake. The module should be installed in `src/modules/balancing`.
3. Copy the configuration file from `src/modules/achievements/src/balancing.conf.dist.in` and place it where your mangosd executable is. Also rename it to `balancing.conf`.
4. Remember to edit the config file and modify the options you want to use.
5. Copy the patch files from the folder `src/modules/balancing/patch` to your client Data folder. Select the locale based on your client locale. If another patch already exists with that name you can rename it with a letter or number (e.g. patch-13.mpq or patch-W.mpq)
6. You will also have to install the database changes located in the `src/balancing/balancing/sql/install` folder, each folder inside represents where you should execute the queries. E.g. The queries inside of `src/modules/balancing/sql/install/world` will need to be executed in the world/mangosd database, the ones in `src/modules/balancing/sql/install/characters` in the characters database, etc...

# How to uninstall
To remove transmog from your server you have to remove it from the db and client:
1. Remove the `BUILD_MODULE_BALANCING` flag from your cmake configuration and recompile the game
2. Execute the sql queries located in the `src/modules/balancing/sql/uninstall` folder. Each folder inside represents where you should execute the queries. E.g. The queries inside of `src/modules/balancing/sql/uninstall/world` will need to be executed in the world/mangosd database, the ones in `src/modules/balancing/sql/uninstall/characters` in the characters database, etc...
3. Remove the patch files from the client added in the installation phase.

# Planned work
- Spells:
	- Shaman
		- Add Shamanistic Rage: https://www.wowhead.com/classic/spell=425336