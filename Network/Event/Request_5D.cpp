#include "Request_5D.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/RealmUser.h"
#include "../../Database/Database.h"
#include "../../logging.h"

#include "../../Common/RLEZ.hpp"

void Request_5D::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	auto sessionId = stream->read_encrypted_utf16();
	auto length = stream->read_u32();
	m_data = stream->read_bytes( length );
	auto unknown = stream->read_u32();
}

sptr_generic_response Request_5D::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = RealmUserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< Result_5D >( this, FATAL_ERROR );
	}

	//auto unpacked = RLEZ::Decompress( m_data );

	//Log::PacketToFile( "5D_data_comp", m_data, m_data.size());
	//Log::PacketToFile( "5D_data_decomp", unpacked, unpacked.size());

	//auto &myCharacter = user->m_character;

	//if( nullptr == myCharacter )
	//{
	//	return std::make_shared< Result_5D >( this, FATAL_ERROR );
	//}

	//myCharacter->SetInventoryData( m_data );

	//auto result = Database::Get().SaveInventory( user->m_accountId, user->m_characterId, m_data );

	//if( !result )
	//{
	//	Log::Error( "Failed to save inventory for account ID: " + std::to_string( user->m_accountId ) + ", character ID: " + std::to_string( user->m_characterId ) );
	//	return std::make_shared< Result_5D >( this, FATAL_ERROR );
	//}

	return std::make_shared< Result_5D >( this, SUCCESS );
}

Result_5D::Result_5D( GenericRequest *request, int32_t reply ) : GenericResponse( *request )
{
	m_reply = reply;
}

ByteBuffer& Result_5D::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( m_reply );

	return m_stream;
}
