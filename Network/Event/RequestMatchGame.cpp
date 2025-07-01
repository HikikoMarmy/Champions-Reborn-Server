#include "RequestMatchGame.h"

#include <format>

#include "../../Common/Constant.h"
#include "../../Game/RealmUserManager.h"
#include "../../Game/GameSessionManager.h"
#include "../../Game/RealmUser.h"

void RequestMatchGame::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();

	auto unknown_a = stream->read_u16();
	auto unknown_b = stream->read_u32();
	auto unknown_c = stream->read_u32();
	auto unknown_d = stream->read_u32();

	auto unknown_e = stream->read_u32();

	// Match Game Node Count
	for( uint32_t i = 0; i < unknown_e; i++ )
	{
		auto node_a = stream->read_u16();
		auto node_b = stream->read_u32();

		auto node_c = stream->read_utf16();

		auto node_d = stream->read_u32();
		auto node_e = stream->read_u32();
		auto node_f = stream->read_u32();
		auto node_g = stream->read_u16();
	}

	auto unknown_f = stream->read_u8();
	auto unknown_g = stream->read_u32();
	auto unknown_h = stream->read_u32();
}

sptr_generic_response RequestMatchGame::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	return std::make_shared< ResultMatchGame >( this, socket->remote_ip );
}

ResultMatchGame::ResultMatchGame( GenericRequest *request, std::string userIp ) : GenericResponse( *request )
{
	m_userIp = userIp;
}

void ResultMatchGame::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( 0 );

	const auto publicGameList = GameSessionManager::Get().GetAvailableGameSessionList( RealmGameType::CHAMPIONS_OF_NORRATH );
	const auto publicGameCount = static_cast< uint32_t >( publicGameList.size() );

	out.write_u32( publicGameCount );
	{
		for( const auto &game : publicGameList )
		{
			if( m_userIp == game->m_hostExternalAddr )
				out.write_utf16( std::format( L"{}:{}", Util::UTF8ToWide( game->m_hostLocalAddr ), game->m_hostNatPort ) );
			else
				out.write_utf16( std::format( L"{}:{}", Util::UTF8ToWide( game->m_hostExternalAddr ), game->m_hostNatPort ) );
		}
	}

	out.write_u32( publicGameCount );
	{
		for( const auto &game : publicGameList )
			out.write_utf16( game->m_gameName );
	}

	out.write_u32( publicGameCount );
	{
		for( const auto &game : publicGameList )
			out.write_utf16( game->m_ownerName );
	}

	out.write_u32( publicGameCount );
	{
		for( const auto &game : publicGameList )
			out.write_u32( game->m_gameId );
	}

	out.write_u32( publicGameCount );
	{
		for( const auto &game : publicGameList )
			out.write_utf8( game->m_gameData );
	}
}
