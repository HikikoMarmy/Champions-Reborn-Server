#include "RequestSaveCharacter_RTA.h"

#include "../../Database/Database.h"
#include "../../Game/GameSessionManager.h"
#include "../../Game/CharacterSaveManager.h"
#include "../../Game/RealmUserManager.h"
#include "../../Game/RealmUser.h"
#include "../../Game/RealmCharacter.h"
#include "../../logging.h"

#include "../../Common/RLEZ.hpp"

void RequestSaveCharacter_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_memberSessionId = stream->read_encrypted_utf16();
	m_targetCharacterId = stream->read_u32();

	auto a = stream->read_u32();
	auto b = stream->read_u32();

	m_metaData.Deserialize( stream );
	auto characterDataSize = stream->read_u32();
	m_characterData = stream->read_bytes( characterDataSize );
}

sptr_generic_response RequestSaveCharacter_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto &userManager = UserManager::Get();

	auto user = userManager.FindUserBySocket( socket );
	if( user == nullptr || user->m_accountId == -1 )
	{
		// Try to recover the user through session records.
		user = UserManager::Get().RecoverUserBySession( m_sessionId, socket );

		if( user == nullptr )
		{
			Log::Error( "Failed to recover user by session ID: {}", m_sessionId );
			return std::make_shared< ResultSaveCharacter_RTA >( this, FATAL_ERROR );
		}

		Log::Debug( "Recovered user by session ID: {}", m_sessionId );
	}

	sptr_user targetUser = user;

	if( m_sessionId != m_memberSessionId )
	{
		targetUser = userManager.FindUserBySessionId( m_memberSessionId );
	}
	
	if( targetUser == nullptr || targetUser->m_accountId == -1 )
	{
		Log::Error( "Target user not found or invalid account ID for session: {}", m_memberSessionId );
		return std::make_shared< ResultSaveCharacter_RTA >( this, FATAL_ERROR );
	}

	auto characterId = targetUser->m_characterId;

	auto &saveManager = CharacterSaveManager::Get();
	if( saveManager.BeginSaveTask( user, targetUser, characterId, m_metaData, CharacterSaveType::SAVE_CHARACTER ) )
	{
		saveManager.AppendSaveData( user->m_sessionId, m_characterData, false );
	}

	return std::make_shared< ResultSaveCharacter_RTA >( this, SUCCESS );
}

ResultSaveCharacter_RTA::ResultSaveCharacter_RTA( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

void ResultSaveCharacter_RTA::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );
}
