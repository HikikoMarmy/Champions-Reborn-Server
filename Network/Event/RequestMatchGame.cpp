#include "../../global_define.h"
#include "RequestMatchGame.h"

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

sptr_generic_response RequestMatchGame::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	return std::make_shared< ResultMatchGame >( this );
}

ResultMatchGame::ResultMatchGame( GenericRequest *request ) : GenericResponse( *request )
{

}

ByteStream &ResultMatchGame::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	auto publicGameList = GameSessionManager::Get().GetAvailableGameSessionList( RealmClientType::CHAMPIONS_OF_NORRATH );
	auto publicGameCount = static_cast< uint32_t >( publicGameList.size() );

	m_stream.write_u32( publicGameCount );
	{
		for( auto &game : publicGameList )
			m_stream.write_utf16( std::format( L"{}:{}", game->m_hostExternalAddr, game->m_hostPort ) );
	}

	m_stream.write_u32( publicGameCount );
	{
		for( auto &game : publicGameList )
			m_stream.write_utf16( game->m_gameName );
	}

	m_stream.write_u32( publicGameCount );
	{
		for( auto &game : publicGameList )
			m_stream.write_utf16( game->m_ownerName );
	}

	m_stream.write_u32( publicGameCount );
	{
		for( auto &game : publicGameList )
			m_stream.write_u32( game->m_gameIndex );
	}

	m_stream.write_u32( publicGameCount );
	{
		for( auto &game : publicGameList )
			m_stream.write_utf8( game->m_gameData );
	}

	return m_stream;
}
