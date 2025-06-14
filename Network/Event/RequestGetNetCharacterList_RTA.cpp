#include "RequestGetNetCharacterList_RTA.h"

#include "../../Game/RealmUser.h"
#include "../../Game/RealmCharacterMetaKV.h"
#include "../../Database/Database.h"
#include "../../logging.h"

void RequestGetNetCharacterList_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestGetNetCharacterList_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	//auto result = Database::Get().LoadCharacterList( user->m_accountId );

	auto result = std::map< int32_t, RealmCharacterMetaData >();

	return std::make_shared< ResultGetNetCharacterList_RTA >( this, SUCCESS, result );
}

ResultGetNetCharacterList_RTA::ResultGetNetCharacterList_RTA( GenericRequest *request, int32_t reply, std::map< int32_t, RealmCharacterMetaData > list ) : GenericResponse( *request )
{
	m_reply = reply;
	m_characterList = std::move( list );
}

ByteBuffer &ResultGetNetCharacterList_RTA::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( m_reply );

	if( 0 == m_reply )
	{
		// Character Count
		m_stream.write_u32( static_cast< int >( m_characterList.size() ) );

		// Character List
		for( auto &character : m_characterList )
		{
			const auto &KV = character.second.GetMetaData();

			// Character ID
			m_stream.write_u32( character.first );

			// Number of Key-Value pairs
			m_stream.write_u32( static_cast< uint32_t >( KV.size() ) );

			for( auto &pair : KV )
			{
				m_stream.write_utf16( pair.first );
				m_stream.write_utf16( pair.second );
			}
		}
	}
	else
	{
		m_stream.write_u32( 0 ); // No characters
	}

	return m_stream;
}
