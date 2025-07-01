#include "RealmCharacter.h"

#include "CharacterSaveTask.h"
#include "../Common/ByteBufferReader.hpp"
#include "../Common/RLEZ.hpp"

#include "../../logging.h"

RealmCharacter::RealmCharacter()
{
	m_characterId = 0;
	m_data.clear();

	Initialize();
}

RealmCharacter::RealmCharacter( const int32_t character_id, const CharacterSlotData meta, const std::vector<uint8_t> data )
{
	m_characterId = character_id;
	m_data = data;
	m_metaData = meta;

	Initialize();
}

RealmCharacter::~RealmCharacter()
{
}

void RealmCharacter::Initialize()
{
	name = "";
	for( auto &val : unknown_000 )
	{
		val = 0;
	}

	unknown_str = "";

	for( auto &val : unknown_004 )
	{
		val = 0;
	}

	character_class = CharacterClass::WARRIOR;
	character_race = CharacterRace::BARBARIAN_M;

	current_level = 0;
	pending_level = 0;

	unknown_009 = 0;
	experience = 0;

	std::memset( unknown_010, 0, sizeof( unknown_010 ) );
	std::memset( stats, 0, sizeof( stats ) );

	unknown_016 = 0;
	current_hp = 0.0f;
	maximum_hp = 0.0f;

	unknown_017 = 0;
	unknown_018 = 0;
	unknown_019 = 0;

	current_mana = 0.0f;
	maximum_mana = 0.0f;

	unknown_020 = 0;

	attack_power = 0;
	minimum_damage = 0;
	maximum_damage = 0;

	unknown_021 = 0;
	unknown_022 = 0;
	unknown_023 = 0;
	unknown_024 = 0;
	unknown_025 = 0;
	unknown_026 = 0;

	current_gold = 0;
	current_skill_points = 0;
	current_ability_points = 0;
	has_spent_remaining_points = 0;

	unknown_029 = 0;
	unknown_030 = 0;
	unknown_031 = 0;
	unknown_032 = 0;
	unknown_033 = 0;
	unknown_034 = 0;
	unknown_035 = 0;
	unknown_036 = 0;
	unknown_037 = 0;
	unknown_038 = 0;
	unknown_039 = 0;
	unknown_040 = 0;

	weight = 0.0f;
	max_weight = 0.0f;

	unknown_041 = 0;
	unknown_042 = 0;

	item_data.fill( ItemData() );
	num_armor_item = 0;
	unknown_043 = 0;
	armor_item_data.fill( ItemData() );
	num_weapon_item = 0;
	unknown_044 = 0;
	weapon_item_data.fill( ItemData() );
	num_consumable_item = 0;

	unknown_045 = 0;
	unknown_046.fill( 0 );

	quest_string.fill( s_quest() );
	num_quests = 0;

	unknown_048 = 0;
	unknown_049 = 0;
	unknown_050 = 0;
	unknown_051 = 0;
	unknown_052 = 0;
	unknown_053 = 0;
	unknown_054 = 0;

	equipment.fill( 0 );

	newStyle.fill( s_new_style() );

	unknown_075 = 0;
	unknown_076 = 0.0f;

	unknown_077 = 0;
	unknown_078 = 0;
	unknown_079 = 0;
	unknown_080 = 0;
	unknown_081 = 0;
	unknown_082 = 0;
	unknown_083 = 0;
	unknown_084 = 0;
	unknown_085 = 0;

	skill_slot[ 0 ] = 0;
	skill_slot[ 1 ] = 0;

	unknown_087 = 0;
	unknown_088 = 0;

	attackData.fill( AttackData() );

	skills.fill( s_skill() );

	unknown_091 = 0;

	mission_progress.fill( s_difficulty_progress() );
	mission_medals.fill( 0 );

	evil_bitflag = 0;
	good_bitflag = 0;
	unknown_101 = 0;
	movement_speed = 0.0f;

	unknown_102 = 0;
	unknown_103 = 0;
	unknown_104 = 0;
	unknown_105 = 0;
	unknown_106 = 0;
	unknown_107 = 0;
	unknown_108 = 0;
	unknown_109 = 0;
	unknown_110 = 0;
}

const std::vector< uint8_t > RealmCharacter::Serialize() const
{
	auto writer = ByteBuffer( CHARACTER_DATA_SIZE );

	/*writer.write_sz_utf8(name, 32);

	for( const auto &val : unknown_000 )
	{
		writer.write_u8( val );
	}

	writer.write_sz_utf8( unknown_str, 32 );

	for( const auto &val : unknown_004 )
	{
		writer.write_i32( val );
	}

	writer.write_i32( static_cast< int32_t >( character_class ) );
	writer.write_i32( static_cast< int32_t >( character_race ) );

	writer.write_u8( current_level );
	writer.write_u8( pending_level );
	writer.write_u16( unknown_009 );
	writer.write_i32( experience );

	for( const auto &val : unknown_010 )
	{
		writer.write_i32( val );
	}

	for( const auto &stat : stats )
	{
		writer.write( stat );
	}

	writer.write_i32( unknown_016 );
	writer.write_f32( current_hp );
	writer.write_f32( maximum_hp );
	writer.write_i32( unknown_017 );
	writer.write_i32( unknown_018 );
	writer.write_i32( unknown_019 );
	writer.write_f32( current_mana );
	writer.write_f32( maximum_mana );
	writer.write_i32( unknown_020 );
	writer.write_i32( attack_power );
	writer.write_i32( minimum_damage );
	writer.write_i32( maximum_damage );
	writer.write_i32( unknown_021 );
	writer.write_i32( unknown_022 );
	writer.write_u8( unknown_023 );
	writer.write_u8( unknown_024 );
	writer.write_u8( unknown_025 );
	writer.write_u8( unknown_026 );
	writer.write_i32( current_gold );
	writer.write_i32( current_skill_points );
	writer.write_i16( current_ability_points );
	writer.write_i16( has_spent_remaining_points );

	writer.write_i32( unknown_029 );
	writer.write_i32( unknown_030 );
	writer.write_i32( unknown_031 );
	writer.write_i32( unknown_032 );
	writer.write_i32( unknown_033 );
	writer.write_i32( unknown_034 );
	writer.write_i32( unknown_035 );
	writer.write_i32( unknown_036 );
	writer.write_i32( unknown_037 );
	writer.write_i32( unknown_038 );
	writer.write_i32( unknown_039 );
	writer.write_i32( unknown_040 );

	writer.write_f32( weight );
	writer.write_f32( max_weight );

	writer.write_i32( unknown_041 );
	writer.write_i32( unknown_042 );

	for( const auto &item : item_data )
	{
		writer.write( item );
	}

	writer.write_i32( num_armor_item );
	writer.write_i32( unknown_043 );
	for( const auto &item : armor_item_data )
	{
		writer.write( item );
	}

	writer.write_i32( num_weapon_item );
	writer.write_i32( unknown_044 );
	for( const auto &item : weapon_item_data )
	{
		writer.write( item );
	}

	writer.write_i32( num_consumable_item );
	writer.write_i32( unknown_045 );

	writer.forward( 2696 );

	for( const auto &quest : quest_string )
	{
		writer.write_sz_utf8( quest.name, 32 );
		writer.write_sz_utf8( quest.description, 32 );
	}

	writer.write_i32( num_quests );

	writer.write_i32( unknown_048 );
	writer.write_i32( unknown_049 );
	writer.write_i32( unknown_050 );
	writer.write_i32( unknown_051 );
	writer.write_i32( unknown_052 );
	writer.write_i32( unknown_053 );
	writer.write_i32( unknown_054 );

	for( const auto &slot : equipment )
	{
		writer.write_i32( slot );
	}

	for( const auto &style : newStyle )
	{
		writer.write< s_new_style >( style );
	}

	writer.write_i32( unknown_075 );
	writer.write_f32( unknown_076 );

	writer.write_i32( unknown_077 );
	writer.write_i32( unknown_078 );
	writer.write_i32( unknown_079 );
	writer.write_i32( unknown_080 );
	writer.write_i32( unknown_081 );
	writer.write_i32( unknown_082 );
	writer.write_i32( unknown_083 );
	writer.write_i32( unknown_084 );
	writer.write_i32( unknown_085 );

	for( const auto &skill : skill_slot )
	{
		writer.write_u8( skill );
	}

	writer.write_u8( unknown_087 );
	writer.write_u8( unknown_088 );

	for( const auto &attack : attackData )
	{
		writer.write< AttackData >( attack );
	}

	for( const auto &skill : skills )
	{
		writer.write< s_skill >( skill );
	}

	writer.write_i32( unknown_091 );

	for( const auto &progress : mission_progress )
	{
		writer.write< s_difficulty_progress >( progress );
	}

	for( const auto &medal : mission_medals )
	{
		writer.write_u8( medal );
	}

	writer.write_u8( evil_bitflag );
	writer.write_u8( good_bitflag );

	writer.write_i32( unknown_101 );

	writer.write_f32( movement_speed );

	writer.write_u8( unknown_102 );
	writer.write_u8( unknown_103 );
	writer.write_u8( unknown_104 );
	writer.write_u8( unknown_105 );
	writer.write_u8( unknown_106 );
	writer.write_u8( unknown_107 );
	writer.write_u8( unknown_108 );
	writer.write_u8( unknown_109 );

	writer.write_i32( unknown_110 );*/

	return writer.get_buffer();
}

void RealmCharacter::Deserialize( const std::vector<uint8_t> &data )
{
	ByteBufferReader reader( data );

	/*try
	{
		name = reader.readString( 32 );

		for( auto &val : unknown_000 )
		{
			val = reader.read< uint8_t >();
		}

		unknown_str = reader.readString( 32 );

		for( auto &val : unknown_004 )
		{
			val = reader.read< int32_t >();
		}

		character_class = static_cast< CharacterClass >( reader.read< int32_t >() );
		character_race = static_cast< CharacterRace >( reader.read< int32_t >() );

		current_level = reader.read< uint8_t >();
		pending_level = reader.read< uint8_t >();
		unknown_009 = reader.read< uint16_t >();
		experience = reader.read< int32_t >();

		for( auto &val : unknown_010 )
		{
			val = reader.read< int32_t >();
		}

		for( auto &stat : stats )
		{
			stat = reader.read< s_stats >();
		}

		unknown_016 = reader.read< int32_t >();

		current_hp = reader.read< float_t >();
		maximum_hp = reader.read< float_t >();

		unknown_017 = reader.read< int32_t >();
		unknown_018 = reader.read< int32_t >();
		unknown_019 = reader.read< int32_t >();

		current_mana = reader.read< float_t >();
		maximum_mana = reader.read< float_t >();

		unknown_020 = reader.read< int32_t >();

		attack_power = reader.read< int32_t >();
		minimum_damage = reader.read< int32_t >();
		maximum_damage = reader.read< int32_t >();

		unknown_021 = reader.read< int32_t >();
		unknown_022 = reader.read< int32_t >();
		unknown_023 = reader.read< uint8_t >();
		unknown_024 = reader.read< uint8_t >();
		unknown_025 = reader.read< uint8_t >();
		unknown_026 = reader.read< uint8_t >();

		current_gold = reader.read< int32_t >();
		current_skill_points = reader.read< int32_t >();
		current_ability_points = reader.read< int16_t >();
		has_spent_remaining_points = reader.read< int16_t >();

		//
		unknown_029 = reader.read< int32_t >();
		unknown_030 = reader.read< int32_t >();
		unknown_031 = reader.read< int32_t >();
		unknown_032 = reader.read< int32_t >();

		//
		unknown_033 = reader.read< int32_t >();
		unknown_034 = reader.read< int32_t >();
		unknown_035 = reader.read< int32_t >();
		unknown_036 = reader.read< int32_t >();

		//
		unknown_037 = reader.read< int32_t >();
		unknown_038 = reader.read< int32_t >();
		unknown_039 = reader.read< int32_t >();
		unknown_040 = reader.read< int32_t >();

		weight = reader.read< float_t >();
		max_weight = reader.read< float_t >();

		unknown_041 = reader.read< int32_t >();
		unknown_042 = reader.read< int32_t >();

		item_data = reader.readArray< ItemData, 64>();

		num_armor_item = reader.read< int32_t >();
		unknown_043 = reader.read< int32_t >();
		armor_item_data = reader.readArray< ItemData, 64>();

		num_weapon_item = reader.read< int32_t >();
		unknown_044 = reader.read< int32_t >();
		weapon_item_data = reader.readArray< ItemData, 64>();

		num_consumable_item = reader.read< int32_t >();
		unknown_045 = reader.read< int32_t >();
		unknown_046.fill( 0x00 ); // Fill with zeros, size is 2696 bytes

		for( auto &quest : quest_string )
		{
			quest = reader.read< s_quest >();
		}
		num_quests = reader.read< int32_t >();

		unknown_048 = reader.read< int32_t >();
		unknown_049 = reader.read< int32_t >();
		unknown_050 = reader.read< int32_t >();
		unknown_051 = reader.read< int32_t >();
		unknown_052 = reader.read< int32_t >();
		unknown_053 = reader.read< int32_t >();
		unknown_054 = reader.read< int32_t >();

		for( auto &slot : equipment )
		{
			slot = reader.read< int32_t >();
		}

		for( auto &style : newStyle )
		{
			style = reader.read< s_new_style >();
		}

		unknown_075 = reader.read< int32_t >();
		unknown_076 = reader.read< float_t >();
		unknown_077 = reader.read< int32_t >();
		unknown_078 = reader.read< int32_t >();
		unknown_079 = reader.read< int32_t >();
		unknown_080 = reader.read< int32_t >();
		unknown_081 = reader.read< int32_t >();
		unknown_082 = reader.read< int32_t >();
		unknown_083 = reader.read< int32_t >();
		unknown_084 = reader.read< int32_t >();
		unknown_085 = reader.read< int32_t >();

		for( auto &skill : skill_slot )
		{
			skill = reader.read< uint8_t >();
		}

		unknown_087 = reader.read< uint8_t >();
		unknown_088 = reader.read< uint8_t >();

		for( auto &attack : attackData )
		{
			attack = reader.read< AttackData >();
		}

		for( auto &skill : skills )
		{
			skill.skill_id = reader.read< int16_t >();
			skill.skill_level = reader.read< int16_t >();
		}

		unknown_091 = reader.read< int32_t >();

		for( auto &val : mission_progress )
		{
			val = reader.read< s_difficulty_progress >();
		}

		for( auto &val : mission_medals )
		{
			val = reader.read< uint8_t >();
		}

		evil_bitflag = reader.read< uint8_t >();
		good_bitflag = reader.read< uint8_t >();

		unknown_101 = reader.read< int32_t >();
		movement_speed = reader.read< float_t >();

		unknown_102 = reader.read< uint8_t >();
		unknown_103 = reader.read< uint8_t >();
		unknown_104 = reader.read< uint8_t >();
		unknown_105 = reader.read< uint8_t >();
		unknown_106 = reader.read< uint8_t >();
		unknown_107 = reader.read< uint8_t >();
		unknown_108 = reader.read< uint8_t >();
		unknown_109 = reader.read< uint8_t >();

		unknown_110 = reader.read< int32_t >();
	}
	catch( const std::out_of_range &e )
	{
		Log::Error( "Failed to deserialize RealmCharacter: %s", e.what() );
		return;
	}*/
}

bool RealmCharacter::ValidateData()
{
	if( m_data.empty() || m_data.size() < CHARACTER_DATA_SIZE )
	{
		Log::Error( "Character data is invalid or too small!" );
		return false;
	}

	if( current_level < 1 || current_level > 80 )
	{
		Log::Error( "Invalid character level: %d", current_level );
		return false;
	}

	if( current_hp < 1.0f || current_hp > maximum_hp )
	{
		Log::Error( "Invalid HP values: current %f, maximum %f", current_hp, maximum_hp );
		return false;
	}

	if( current_mana < 0.0f || current_mana > maximum_mana )
	{
		Log::Error( "Invalid mana values: current %f, maximum %f", current_mana, maximum_mana );
		return false;
	}

	if( weight < 0.0f || weight > max_weight )
	{
		Log::Error( "Invalid weight values: current %f, maximum %f", weight, max_weight );
		return false;
	}

	if( movement_speed <= 0.0f )
	{
		Log::Error( "Invalid movement speed: %f", movement_speed );
		return false;
	}

	for( const auto &mission : mission_progress )
	{
		if( mission.mission_na < 0 || mission.mission_na > 3 )
		{
			Log::Error( "Invalid mission NA value: %d", mission.mission_na );
			return false;
		}

		if( mission.mission_Disease < 0 || mission.mission_Disease > 3 )
		{
			Log::Error( "Invalid mission Disease value: %d", mission.mission_Disease );
			return false;
		}

		if( mission.mission_Fear < 0 || mission.mission_Fear > 3 )
		{
			Log::Error( "Invalid mission Fear value: %d", mission.mission_Fear );
			return false;
		}
	}

	return true;
}

CharacterSlotData RealmCharacter::GetMetaData() const
{
	return m_metaData;
}

void RealmCharacter::SetMetaData( sptr_byte_stream stream )
{
	m_metaData.Deserialize( stream );
}

std::vector<uint8_t> RealmCharacter::Unpack()
{
	std::vector< uint8_t > output;
	size_t read = 4;

	while( read < m_data.size() )
	{
		uint8_t byte = m_data[ read++ ];
		output.push_back( byte );

		if( byte == 0x00 )
		{
			if( read >= m_data.size() )
			{
				break;
			}

			uint8_t count = m_data[ read++ ];
			output.insert( output.end(), count, 0x00 );
		}
	}

	return output;
}
