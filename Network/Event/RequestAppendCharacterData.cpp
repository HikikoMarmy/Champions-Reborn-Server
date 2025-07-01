#include "RequestAppendCharacterData.h"

#include "../../Game/CharacterSaveManager.h"
#include "../../Game/RealmUserManager.h"
#include "../../Game/RealmUser.h"
#include "../../Database/Database.h"
#include "../../logging.h"

void RequestAppendCharacterData::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	auto sessionId = stream->read_encrypted_utf16();

	auto length = stream->read_u32();
	m_data = stream->read_bytes( length );
	m_endOfData = stream->read_u32();
}

sptr_generic_response RequestAppendCharacterData::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< ResultAppendCharacterData >( this, FATAL_ERROR );
	}

	CharacterSaveManager::Get().AppendSaveData( user->m_sessionId, m_data, m_endOfData );

	return std::make_shared< ResultAppendCharacterData >( this, SUCCESS );
}

ResultAppendCharacterData::ResultAppendCharacterData( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

void ResultAppendCharacterData::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );
}
