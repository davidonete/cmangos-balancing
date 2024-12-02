# Balancing
Module to balance classes a little bit better by adding new spells and modifying existing ones. The added spells will be spread across the world in the form of books that can be looted.

List of changes:
- Spells:
	- Paladin
		- Added Crusader Strike (33393): https://www.wowhead.com/wotlk/spell=35395
		- Added Hand of Reckoning (33395): https://www.wowhead.com/wotlk/spell=62124
		- Modified Benediction (20101, 20102, 20103, 20104, 20105) to increase mana reduction up to 30%
		- Modified Holy Shield (20925, 20927, 20928) to increase block chance to 50% and reduce a 50% the mana cost
		- Modified Consecration (26573, 20116, 20922, 20923, 20924) to reduce mana cost a 50%
		- Modified Blessings increase duration a 100%
		- Modified Hammer of Wrath (24239, 24274, 24275) cast time instant
		- Modified Judgement different debuffs can be stacked
	- Druid
		- Added Starfall (33397): https://www.wowhead.com/classic/spell=439748
		- Modified Natural Shapeshifter (16833, 16834, 16835) to increase mana reduction up to 60%
		- Modified Tiger's Fury (5217, 6793, 9845, 9846) to remove energy cost and add a 30 seconds cooldown
		- Modified Savage Fury (16998, 16999) will also increase bleed damage up to a 30%
	- Shaman
		- Modified Convection (16039, 16109, 16110, 16111, 16112) to increase mana reduction up to 30%

# Available Cores
Classic

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