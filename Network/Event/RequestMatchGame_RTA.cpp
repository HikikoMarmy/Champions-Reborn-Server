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

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		UserManager::Get().Disconnect( socket, "User not found!" );
		return std::make_shared< ResultMatchGame_RTA >( this, "" );
	}

	if( !user->m_isLoggedIn )
	{
		UserManager::Get().Disconnect( user, "User is not logged in!" );
		return std::make_shared< ResultMatchGame_RTA >( this, "" );
	}

	return std::make_shared< ResultMatchGame_RTA >( this, socket->remote_ip );
}

ResultMatchGame_RTA::ResultMatchGame_RTA( GenericRequest *request, std::string userIp ) : GenericResponse( *request )
{
	m_userIp = userIp;
}

void ResultMatchGame_RTA::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( 0 );

	const auto publicGameList = GameSessionManager::Get().GetAvailableGameSessionList( RealmGameType::RETURN_TO_ARMS );
	const auto publicGameCount = static_cast< uint32_t >( publicGameList.size() );

	out.write_u32( publicGameCount );
	{
		for( const auto &game : publicGameList )
			out.write_utf16( Util::UTF8ToWide( game->m_gameData ) );
	}

	out.write_u32( publicGameCount );
	{
		for( const auto &game : publicGameList )
			out.write_utf16( game->m_playerCount );
	}

	out.write_u32( publicGameCount );
	{
		for( const auto &game : publicGameList )
			out.write_u32( game->m_gameId );
	}

	// Something about filtering.
	out.write_u32( publicGameCount );
	{
		out.write_u32( 0 );	// Size
	}

	out.write_u32(publicGameCount);
	{
		for (const auto& game : publicGameList)
			out.write_utf16(Util::UTF8ToWide(game->m_hostExternalAddr));
	}

	out.write_u32(publicGameCount);
	{
		for (const auto& game : publicGameList)
			out.write_u32(game->m_hostNatPort);
	}

	out.write_u32(publicGameCount);
	{
		for (const auto& game : publicGameList)
			out.write_utf16(Util::UTF8ToWide(game->m_hostLocalAddr));
	}

	out.write_u32(publicGameCount);
	{
		for (const auto& game : publicGameList)
			out.write_u32(game->m_hostLocalPort);
	}
}
