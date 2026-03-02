#include "Network/Event/RequestCreateNewCharacter_RTA.hpp"

#include "Database/Database.hpp"
#include "Game/CharacterSaveManager.hpp"
#include "Game/RealmUserManager.hpp"
#include "Game/RealmUser.hpp"
#include "Game/RealmCharacter.hpp"
#include "logging.hpp"

void RequestCreateNewCharacter_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();

	auto a = stream->read_u32();
	auto b = stream->read_u32();

	m_metaData.Deserialize( stream );

	auto characterDataSize = stream->read_u32();
	m_characterData = stream->read_bytes( characterDataSize );
}

sptr_generic_response RequestCreateNewCharacter_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "User not found! [{}]", m_sessionId );
		return std::make_shared< ResultCreateNewCharacter_RTA >( this, FATAL_ERROR );
	}

	if( user->m_gameType != RealmGameType::RETURN_TO_ARMS )
	{
		Log::Error( "Invalid game type for CreateNewCharacter_RTA request! [{}]", m_sessionId );
		return std::make_shared< ResultCreateNewCharacter_RTA >( this, FATAL_ERROR );
	}

	auto &saveManager = CharacterSaveManager::Get();
	if( !saveManager.BeginSaveTask( user, user, 0, m_metaData, CharacterSaveType::NEW_CHARACTER ) )
	{
		Log::Error( "Failed to begin save task for new character! [{}]", m_sessionId );
		return std::make_shared< ResultCreateNewCharacter_RTA >( this, FATAL_ERROR );
	}

	saveManager.AppendSaveData( user->m_sessionId, m_characterData, false );

	return std::make_shared< ResultCreateNewCharacter_RTA >( this, SUCCESS );
}

ResultCreateNewCharacter_RTA::ResultCreateNewCharacter_RTA( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

void ResultCreateNewCharacter_RTA::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );
}
