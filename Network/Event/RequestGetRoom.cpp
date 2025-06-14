#include "RequestGetRoom.h"

void RequestGetRoom::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	auto sessionId = stream->read_encrypted_utf16();
}

sptr_generic_response RequestGetRoom::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	return std::make_shared< ResultGetRoom >( this );
}

ResultGetRoom::ResultGetRoom( GenericRequest *request ) : GenericResponse( *request )
{
}

ByteBuffer &ResultGetRoom::Serialize()
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
