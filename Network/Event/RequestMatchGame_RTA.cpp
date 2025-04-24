#include "../../global_define.h"
#include "RequestMatchGame_RTA.h"

void RequestMatchGame_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestMatchGame_RTA::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	return std::make_shared< ResultMatchGame_RTA >( this );
}

ResultMatchGame_RTA::ResultMatchGame_RTA( GenericRequest *request ) : GenericResponse( *request )
{
}

ByteStream&ResultMatchGame_RTA::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	auto publicGameList = GameSessionManager::Get().GetAvailableGameSessionList( RealmClientType::RETURN_TO_ARMS );
	auto publicGameCount = static_cast< uint32_t >( publicGameList.size() );

	m_stream.write_u32( publicGameCount );
	{
		for( auto &game : publicGameList )
			m_stream.write_utf16( game->m_gameName );
	}

	m_stream.write_u32( publicGameCount );
	{
		for( auto &game : publicGameList )
			m_stream.write_utf16( game->m_playerCount );
	}

	m_stream.write_u32( publicGameCount );
	{
		for( auto &game : publicGameList )
			m_stream.write_u32( game->m_gameIndex );
	}

	m_stream.write_u32( publicGameCount );
	{
		m_stream.write_u32( 0 );	// Size
		// Blob Data
	}

	m_stream.write_u32( publicGameCount );
	{
		for( auto &game : publicGameList )
			m_stream.write_utf16( game->m_hostExternalAddr );
	}

	m_stream.write_u32( publicGameCount );
	{
		for( auto &game : publicGameList )
			m_stream.write_u32( game->m_hostPort );
	}

	m_stream.write_u32( publicGameCount );
	{
		for( auto &game : publicGameList )
			m_stream.write_utf16( game->m_hostLocalAddr );
	}

	m_stream.write_u32( publicGameCount );
	{
		for( auto &game : publicGameList )
			m_stream.write_u32( game->m_hostPort );
	}

	return m_stream;
}
