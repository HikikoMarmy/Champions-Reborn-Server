#include "RequestGetCharacterData_RTA.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/RealmUser.h"
#include "../../Database/Database.h"
#include "../../logging.h"

#include "../../Common/RLEZ.hpp"

void RequestGetNetCharacterData_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	auto sessionId = stream->read_encrypted_utf16();
	m_characterId = stream->read_u32();
}

sptr_generic_response RequestGetNetCharacterData_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize(stream);

	auto user = RealmUserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< ResultGetNetCharacterData_RTA >( this, FATAL_ERROR, nullptr );
	}

	auto result = Database::Get().LoadCharacterData( user->m_accountId, m_characterId );

	if( !result )
	{
		Log::Error( "Failed to load character data for account ID: " + std::to_string( user->m_accountId ) + ", character ID: " + std::to_string( m_characterId ) );
		return std::make_shared< ResultGetNetCharacterData_RTA >( this, FATAL_ERROR, nullptr );
	}

	user->m_characterId = m_characterId;
	user->m_character = result;

	auto characterName = result->GetMetaData().GetValue( L"name" );

	Log::Debug( "Character data loaded for account ID: %lld, character ID: %d, name: %ls", user->m_accountId, m_characterId, characterName.c_str() );

	return std::make_shared< ResultGetNetCharacterData_RTA >( this, SUCCESS, result );
}

ResultGetNetCharacterData_RTA::ResultGetNetCharacterData_RTA( GenericRequest *request, int32_t reply, sptr_realm_character data ) : GenericResponse( *request )
{
	m_reply = reply;
	m_data = data;
}

ByteBuffer& ResultGetNetCharacterData_RTA::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( m_reply );

	if( !m_reply )
	{
		const auto &characterData = m_data->m_characterData;

		// Erase first 4 bytes
		//characterData.erase(characterData.begin(), characterData.begin() + 4);

		//const auto unpacked = RLEZ::Decompress(characterData);
		//const auto repacked = RLEZ::Compress(unpacked);

		//Log::PacketToFile( "characterData_1", characterData, characterData.size() );
		//Log::PacketToFile( "characterData_2", unpacked, unpacked.size() );
		//Log::PacketToFile( "characterData_3", repacked, repacked.size() );

		m_stream.write_u32( characterData.size());
		m_stream.write_bytes( characterData );

		for (int i = 0; i < 136; i++)
		{
			m_stream.write_u8(0xFF);
		}
		
		for (int i = 0; i < 5; i++)
		{
			m_stream.write_u32(0);
			m_stream.write_u32(3);
			m_stream.write_u32(0);
			m_stream.write_u8(0);
		}
		
		m_stream.write_u32(0);
		m_stream.write_u32(0);
		m_stream.write_u32(0);
		m_stream.write_u32(0);
		
		m_stream.write_u8(0);
		m_stream.write_u8(5);
		m_stream.write_u8(3);
		
		m_stream.write_u8(0);
		m_stream.write_u8(0);
		m_stream.write_u8(3);
		m_stream.write_u8(1);

		m_stream.write_u32(1);	// End Of Stream
	}

	//Log::Packet( m_stream.get_buffer(), m_stream.get_length(), false);

	return m_stream;
}
