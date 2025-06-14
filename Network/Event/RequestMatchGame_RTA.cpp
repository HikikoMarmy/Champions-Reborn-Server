#include "RequestMatchGame_RTA.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../logging.h"

void RequestMatchGame_RTA::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestMatchGame_RTA::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = RealmUserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< ResultMatchGame_RTA >( this, "" );
	}

	return std::make_shared< ResultMatchGame_RTA >( this, socket->remote_ip );
}

ResultMatchGame_RTA::ResultMatchGame_RTA( GenericRequest *request, std::string userIp ) : GenericResponse( *request )
{
	m_userIp = userIp;
}

ByteBuffer&ResultMatchGame_RTA::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( 0 );

	const auto publicGameList = GameSessionManager::Get().GetAvailableGameSessionList( RealmGameType::RETURN_TO_ARMS );
	const auto publicGameCount = static_cast< uint32_t >( publicGameList.size() );

	m_stream.write_u32( publicGameCount );
	{
		for( const auto &game : publicGameList )
			m_stream.write_utf16( game->m_gameName );
	}

	m_stream.write_u32( publicGameCount );
	{
		for( const auto &game : publicGameList )
			m_stream.write_utf16( game->m_playerCount );
	}

	m_stream.write_u32( publicGameCount );
	{
		for( const auto &game : publicGameList )
			m_stream.write_u32( game->m_gameIndex );
	}

	m_stream.write_u32( publicGameCount );
	{
		m_stream.write_u32( 0 );	// Size
		// Blob Data
	}

	m_stream.write_u32( publicGameCount );
	{
		for( const auto &game : publicGameList )
		{
			if( m_userIp == game->m_hostExternalAddr )
				m_stream.write_utf16( Util::UTF8ToWide( game->m_hostLocalAddr ) );
			else
				m_stream.write_utf16( Util::UTF8ToWide( game->m_hostExternalAddr ) );
		}
	}

	m_stream.write_u32( publicGameCount );
	{
		for( const auto &game : publicGameList )
			m_stream.write_u32( game->m_hostPort );
	}

	m_stream.write_u32( publicGameCount );
	{
		for( const auto &game : publicGameList )
		{
			if( m_userIp == game->m_hostExternalAddr )
				m_stream.write_utf16( Util::UTF8ToWide( game->m_hostLocalAddr ) );
			else
				m_stream.write_utf16( Util::UTF8ToWide( game->m_hostExternalAddr ) );
		}
	}

	m_stream.write_u32( publicGameCount );
	{
		for( const auto &game : publicGameList )
			m_stream.write_u32( game->m_hostPort );
	}

	return m_stream;
}
