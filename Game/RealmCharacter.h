#pragma once

#include <array>
#include <string>
#include <vector>
#include <unordered_map>

#include "../Common/Constant.h"
#include "../Common/ByteStream.h"
#include "RealmCharacterMetaKV.h"

constexpr size_t MAX_NUMBER_OF_CHARACTERS = 12;
constexpr size_t CHARACTER_DATA_SIZE = 19504;

using ItemData = std::array< uint8_t, 72 >;
using AttackData = std::array< float_t, 4 >;

class RealmCharacter {
public:
	RealmCharacter();
    RealmCharacter( const int32_t id, const CharacterSlotData meta, const std::vector< uint8_t > data );
	~RealmCharacter();

    void Initialize();
	const std::vector< uint8_t > Serialize() const;
    void Deserialize( const std::vector< uint8_t > &data );

	bool ValidateData();

	CharacterSlotData GetMetaData() const;
    void SetMetaData( sptr_byte_stream stream );

    std::vector< uint8_t > Unpack();

public:
	uint32_t m_characterId;
    CharacterSlotData m_metaData;
	std::vector< uint8_t > m_data;

public:
    std::string name;
    uint8_t unknown_000[ 4 ];
    std::string unknown_str;
    int32_t unknown_004[ 3 ];

    CharacterClass character_class;
    CharacterRace character_race;

    uint8_t current_level;
    uint8_t pending_level;  // Waiting to spend points, basically.
    uint16_t unknown_009;
    int32_t experience;

    int32_t unknown_010[ 6 ];

    struct s_stats {
        int32_t strength, intelligence, dexterity, stamina, unknown_a, unknown_b;
    } stats[ 2 ];

    int32_t unknown_016;
    float_t current_hp, maximum_hp;
    int32_t unknown_017, unknown_018, unknown_019;
    float_t current_mana, maximum_mana;
    int32_t unknown_020;
    int32_t attack_power, minimum_damage, maximum_damage;
    int32_t unknown_021, unknown_022;

    uint8_t unknown_023, unknown_024, unknown_025, unknown_026;
    int32_t current_gold, current_skill_points;
    int16_t current_ability_points;
    int16_t has_spent_remaining_points;

    int32_t unknown_029, unknown_030, unknown_031, unknown_032;
    int32_t unknown_033, unknown_034, unknown_035, unknown_036;
    int32_t unknown_037, unknown_038, unknown_039, unknown_040;

    float_t weight, max_weight;
    int32_t unknown_041, unknown_042;

    std::array<ItemData, 64> item_data;
    int32_t num_armor_item, unknown_043;
    std::array<ItemData, 64> armor_item_data;

    int32_t num_weapon_item, unknown_044;
    std::array<ItemData, 64> weapon_item_data;

    int32_t num_consumable_item, unknown_045;
    std::array< uint8_t, 2696 > unknown_046;

    struct s_quest {
        char name[ 32 ];
        char description[ 32 ];
    };

    std::array<s_quest, 8> quest_string;

    int32_t num_quests;

    int32_t unknown_048;
    int32_t unknown_049;
    int32_t unknown_050;
    int32_t unknown_051;
    int32_t unknown_052;
    int32_t unknown_053;
    int32_t unknown_054;

    std::array< int32_t, 20 > equipment;

    struct s_new_style {
        char name[ 32 ];    // "newstyle"   
        int id;
        uint8_t active_flag;
        uint8_t type;
        uint8_t unknown_a;
        uint8_t unknown_b;
        uint8_t reserved[ 64 ];
    };

    std::array<s_new_style, 15> newStyle;

    int32_t unknown_075;
    float_t unknown_076;

    int32_t
        unknown_077, unknown_078, unknown_079,
        unknown_080, unknown_081, unknown_082,
        unknown_083, unknown_084, unknown_085;

    uint8_t skill_slot[ 2 ];
    uint8_t unknown_087, unknown_088;

    std::array< AttackData, 8> attackData;

    struct s_skill {
        int16_t skill_id;
        int16_t skill_level;
    };
    std::array<s_skill, 48> skills;

    int32_t unknown_091;

    struct s_difficulty_progress {
        uint8_t mission_na;
        uint8_t mission_War;
        uint8_t mission_Innovation;
        uint8_t mission_PitOfIllOmen;
        uint8_t mission_PlaneOfWater;
        uint8_t mission_Torment;
        uint8_t mission_Disease;
        uint8_t mission_Valor;
        uint8_t mission_Fire;
        uint8_t mission_Storms;
        uint8_t mission_Faydark;
        uint8_t mission_Nightmares;
        uint8_t mission_Fear;
    };

    std::array<s_difficulty_progress, 5> mission_progress;
    std::array< uint8_t, 13 > mission_medals;

    uint8_t evil_bitflag, good_bitflag;
    int32_t unknown_101;
    float_t movement_speed;

    uint8_t unknown_102, unknown_103, unknown_104, unknown_105;
    uint8_t unknown_106, unknown_107, unknown_108, unknown_109;
    int32_t unknown_110;
};

using sptr_realm_character = std::shared_ptr< RealmCharacter >;