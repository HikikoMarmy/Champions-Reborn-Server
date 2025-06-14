#include "RequestCreateNewCharacter_RTA.h"

#include "../../Database/Database.h"
#include "../../Game/RealmUserManager.h"
#include "../../Game/RealmUser.h"
#include "../../Game/RealmCharacter.h"
#include "../../logging.h"

void RequestCreateNewCharacter_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();

	auto a = stream->read_u32(); 
	auto b = stream->read_u32();

	m_newCharacterData = std::make_shared< RealmCharacter >( stream );
}

sptr_generic_response RequestCreateNewCharacter_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = RealmUserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "User not found! [%S]", m_sessionId.c_str() );
		return std::make_shared< ResultCreateNewCharacter_RTA >( this, FATAL_ERROR );
	}

	auto unpacked = m_newCharacterData->Unpack();

	//Log::PacketToFile( "new_character", unpacked, unpacked.size());

	auto characterId = Database::Get().CreateNewCharacter( user->m_accountId, m_newCharacterData->GetMetaData().Serialize(), m_newCharacterData->GetCharacterData() );
	if( characterId < 0 )
	{
		return std::make_shared< ResultCreateNewCharacter_RTA >( this, FATAL_ERROR );
	}

	Log::Info( "New character created for user: " + std::to_string( user->m_accountId ) + ", Character ID: " + std::to_string( characterId ) );
			
	user->m_characterId = characterId;
	user->m_character = m_newCharacterData;

	return std::make_shared< ResultCreateNewCharacter_RTA >( this, SUCCESS );
}

ResultCreateNewCharacter_RTA::ResultCreateNewCharacter_RTA( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

ByteBuffer& ResultCreateNewCharacter_RTA::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( m_reply );

	return m_stream;
}
