#include "RequestGetNetCharacterList_RTA.h"

#include "../../Game/RealmUser.h"
#include "../../Game/RealmUserManager.h"
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

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( !user )
	{
		return std::make_shared< ResultGetNetCharacterList_RTA >( this, FATAL_ERROR, std::nullopt );
	}

	auto result = Database::Get().LoadCharacterSlots( user->m_accountId );

	return std::make_shared< ResultGetNetCharacterList_RTA >( this, SUCCESS, result );
}

ResultGetNetCharacterList_RTA::ResultGetNetCharacterList_RTA( GenericRequest *request, int32_t reply, std::optional< std::map< uint32_t, CharacterSlotData > > list ) : GenericResponse( *request )
{
	m_reply = reply;

	if( list != std::nullopt )
	{
		m_characterList = list.value();
	}
}

void ResultGetNetCharacterList_RTA::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );

	// Character Count
	out.write_u32( static_cast< int >( m_characterList.size() ) );

	// Character List
	for( auto &character : m_characterList )
	{
		const auto &KV = character.second.GetMetaData();

		// Character ID
		out.write_u32( character.first );

		// Number of Key-Value pairs
		out.write_u32( static_cast< uint32_t >( KV.size() ) );

		for( auto &pair : KV )
		{
			out.write_utf16( pair.first );
			out.write_utf16( pair.second );
		}
	}
}
