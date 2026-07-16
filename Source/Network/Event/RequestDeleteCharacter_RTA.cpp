#include "Network/Event/RequestDeleteCharacter_RTA.hpp"

#include "Game/RealmUserManager.hpp"
#include "Game/GameSessionManager.hpp"
#include "Game/RealmUser.hpp"
#include "Database/Database.hpp"
#include "logging.hpp"

void RequestDeleteCharacter_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_characterId = stream->read_u32();
}

sptr_generic_response RequestDeleteCharacter_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "User not found! [{}]", m_sessionId );
		return std::make_shared< ResultDeleteCharacter_RTA >( this, TIMEOUT );
	}

	if (user->m_sessionId != m_sessionId)
	{
		return std::make_shared< ResultDeleteCharacter_RTA >(this, NOT_FOUND);
	}

	const bool result = Database::Get().DeleteCharacter( user->m_accountId, m_characterId );

	if (!result)
	{
		return std::make_shared< ResultDeleteCharacter_RTA >(this, NOT_FOUND);
	}

	return std::make_shared< ResultDeleteCharacter_RTA >( this, SUCCESS );
}

ResultDeleteCharacter_RTA::ResultDeleteCharacter_RTA( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

void ResultDeleteCharacter_RTA::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );
}
