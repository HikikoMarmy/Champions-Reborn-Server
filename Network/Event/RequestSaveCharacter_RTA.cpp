#include "RequestSaveCharacter_RTA.h"

#include "../../Database/Database.h"
#include "../../Game/RealmUserManager.h"
#include "../../Game/RealmUser.h"
#include "../../Game/RealmCharacter.h"
#include "../../logging.h"

#include "../../Common/RLEZ.hpp"

void RequestSaveCharacter_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId_A = stream->read_encrypted_utf16();
	m_sessionId_B = stream->read_encrypted_utf16();
	m_characterId = stream->read_u32(); 

	auto a = stream->read_u32();
	auto b = stream->read_u32();

	m_characterData = std::make_shared< RealmCharacter >( stream );
}

sptr_generic_response RequestSaveCharacter_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = RealmUserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "User not found! [%S]", m_sessionId_A.c_str() );
		return std::make_shared< ResultSaveCharacter_RTA >( this, FATAL_ERROR );
	}

	//auto unpacked = RLEZ::Decompress( m_characterData->GetCharacterData() );

	//Log::PacketToFile( "save_character_comp", stream->get_buffer(), stream->get_length());
	//Log::PacketToFile( "save_character_decomp", unpacked, unpacked.size());

	Log::Packet(stream->get_buffer(), stream->get_length(), false);

	Log::Debug( "Processing RequestSaveCharacter_RTA for user: " + std::to_string( user->m_accountId ) );
	Log::Debug( "Session ID A: " + Util::WideToUTF8( m_sessionId_A ) );
	Log::Debug( "Session ID B: " + Util::WideToUTF8( m_sessionId_B ) );
	Log::Debug( "Character ID: " + std::to_string( m_characterId ) );

	//if( m_characterId != user->m_characterId )
	//{
	//	Log::Error( "Character ID mismatch for user: " + Util::WideToUTF8( user->m_sessionId ) + ". Expected: " + std::to_string(user->m_characterId) + ", Received: " + std::to_string(m_characterId));
	//	return std::make_shared< ResultSaveCharacter_RTA >( this, GENERAL_ERROR );
	//}

	auto result = Database::Get().SaveCharacter( user->m_accountId, user->m_characterId, m_characterData->GetMetaData().Serialize(), m_characterData->GetCharacterData() );

	if( false == result )
	{
		return std::make_shared< ResultSaveCharacter_RTA >( this, FATAL_ERROR );
	}
			
	user->m_character = std::move(m_characterData);

	return std::make_shared< ResultSaveCharacter_RTA >( this, SUCCESS );
}

ResultSaveCharacter_RTA::ResultSaveCharacter_RTA( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

ByteBuffer& ResultSaveCharacter_RTA::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( m_reply );

	return m_stream;
}
