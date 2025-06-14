#include "RequestEnterRoom.h"

void RequestEnterRoom::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestEnterRoom::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto publicKey = stream->read_utf8();
	auto unknown = stream->read_u32();

	return std::make_shared< ResultEnterRoom >( this );
}

ResultEnterRoom::ResultEnterRoom( GenericRequest *request ) : GenericResponse( *request )
{
}

ByteBuffer& ResultEnterRoom::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( 0 );

	m_stream.write_utf16( L"Room Name" );
	m_stream.write_utf16( L"Room Banner" );

	m_stream.write_u32( 3 );
	{
		m_stream.write_utf16( L"Name1" );
		m_stream.write_utf16( L"Name2" );
		m_stream.write_utf16( L"Name3" );
	}

	m_stream.write_u32( 1 );
	{
		m_stream.write_utf16( L"Name1" );
	}

	return m_stream;
}
