#include "../../global_define.h"
#include "RequestMatchGame.h"

void RequestMatchGame::Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();

	auto unknown_a = stream->read_u16();
	auto unknown_b = stream->read_u32();
	auto unknown_c = stream->read_u32();
	auto unknown_d = stream->read_u32();

	auto unknown_e = stream->read_u32();

	// Match Game Node Count
	for( int i = 0; i < unknown_e; i++ )
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

	int dbg = 0;
}

sptr_generic_response RequestMatchGame::ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	Deserialize( socket, stream );

	Log::Debug( "RequestMatchGame : %S", m_sessionId.c_str() );

	return std::make_shared< ResultMatchGame >( this );
}

ResultMatchGame::ResultMatchGame( GenericRequest *request ) : GenericResponse( *request )
{
	
}

ByteStream &ResultMatchGame::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );		// Connection State

	m_stream.write_u32( 5 );

	for( int i = 0; i < 5; i++ )
	{
		m_stream.write_utf16( L"Unknown_A " + std::to_wstring( i ) );
	}

	m_stream.write_u32( 5 );

	for( int i = 0; i < 5; i++ )
	{
		m_stream.write_utf16( L"Game Name " + std::to_wstring( i ) );
	}

	m_stream.write_u32( 5 );

	for( int i = 0; i < 5; i++ )
	{
		m_stream.write_utf16( L"Unknown_B " + std::to_wstring( i ) );
	}


	// Room Unique ID (Used when selecting)
	m_stream.write_u32( 5 );
	for( int i = 0; i < 5; i++ )
	{
		m_stream.write_u32( i );
	}

	return m_stream;
}
