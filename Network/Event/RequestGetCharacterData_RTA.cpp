#include "RequestGetCharacterData_RTA.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/RealmUser.h"
#include "../../Database/Database.h"
#include "../../logging.h"

void RequestGetNetCharacterData_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	auto sessionId = stream->read_encrypted_utf16();
	m_characterId = stream->read_u32();
}

sptr_generic_response RequestGetNetCharacterData_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< ResultGetNetCharacterData_RTA >( this, FATAL_ERROR );
	}

	auto result = Database::Get().LoadCharacterData( user->m_accountId, m_characterId );

	if( !result )
	{
		Log::Error( "Failed to load character data for account ID: " + std::to_string( user->m_accountId ) + ", character ID: " + std::to_string( m_characterId ) );
		return std::make_shared< ResultGetNetCharacterData_RTA >( this, FATAL_ERROR );
	}

	user->m_characterId = result->m_characterId;
	user->m_character = result;

	return SendCharacterData( socket, result );
}

sptr_generic_response RequestGetNetCharacterData_RTA::SendCharacterData( sptr_socket socket, sptr_realm_character character )
{
	const auto &data = character->m_data;
	const auto data_size = data.size();
	size_t position = 0;

	sptr_generic_response finalChunk = nullptr;

	while( position < data_size )
	{
		const size_t chunk_size = std::min<size_t>( 1024, data_size - position );

		std::vector<uint8_t> chunk_data( data.begin() + position, data.begin() + position + chunk_size );

		const int32_t isFinalChunk = ( position + chunk_size >= data_size );

		auto response = std::make_shared<ResultGetNetCharacterData_RTA>( this, SUCCESS, std::move( chunk_data ), isFinalChunk );

		if( isFinalChunk )
		{
			finalChunk = response;
		}
		else
		{
			socket->send( response );
		}

		position += chunk_size;
	}

	return finalChunk;
}

ResultGetNetCharacterData_RTA::ResultGetNetCharacterData_RTA( GenericRequest *request, int32_t reply, std::optional<std::vector<uint8_t>> data, std::optional<int32_t> endOfData ) : GenericResponse( *request )
{
	m_reply = reply;

	if( !data.has_value() )
	{
		m_chunk.clear();
	}
	else
	{
		m_chunk = data.value();
	}

	if( !endOfData.has_value() )
	{
		m_endOfData = 0;
	}
	else
	{
		m_endOfData = endOfData.value();
	}
}

void ResultGetNetCharacterData_RTA::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );

	if( !m_reply )
	{
		out.write_u32( static_cast< uint32_t >( m_chunk.size() ) );
		out.write_bytes( m_chunk );
		out.write_u32( m_endOfData );
	}
}
