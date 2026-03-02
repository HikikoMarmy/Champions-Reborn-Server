#pragma once

constexpr int32_t MAX_SESSION_ID_LENGTH = 16;

enum RealmGameType {
	CHAMPIONS_OF_NORRATH = 0,
	RETURN_TO_ARMS = 1,
};

enum class CharacterClass : int32_t {
    WARRIOR,
    CLERIC,
    SHADOW_KNIGHT,
    RANGER,
    WIZARD,
    BERSERKER,
    SHAMAN,

	NUM_CLASSES
};

enum class CharacterRace : int32_t {
    BARBARIAN_M,
    BARNARIAN_F,
    
    WOOD_ELF_M,
    WOOD_ELF_F,
    
    HIGH_ELF_M,
    HIGH_ELF_F,

    ERUDITE_WIZARD_M,
    ERUDITE_WIZARD_F,
    
    DARK_ELF_M,
    DARK_ELF_F,

    VAH_SHIR_BERSERKER,
    IKSAR_SHAMAN,

	NUM_RACES
};

enum class EquipmentSlot : int32_t {
	PRIMARY_WEAPON = 0,
	UNKNOWN_01 = 1,
	SECONDARY_WEAPON = 2,
	SHIELD = 3,
	TORSO = 4,
	RING_1 = 5,
	RING_2 = 6,
	CHOKER = 7,
	QUIVER = 8,
	GLOVES = 9,
	BOOTS = 10,
	HEAD = 11,
	LEGGINGS = 12,
	UNKNOWN_13 = 13,
	EARRING = 14,
	UNKNOWN_15 = 15,
	UNKNOWN_16 = 16,
	PRIMARY_WEAPON_2 = 17,
	SECONDARY_WEAPON_2 = 18,
	UNKNOWN_19 = 19,
	NUM_EQUIPMENT_SLOTS
};