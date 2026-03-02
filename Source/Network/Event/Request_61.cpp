#include "Request_61.h"

void Request_61::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response Request_61::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto publicKey = stream->read_utf8();
	auto unknown = stream->read_u32();

	return std::make_shared< Result_61 >( this );
}

Result_61::Result_61( GenericRequest *request ) : GenericResponse( *request )
{
}

ByteBuffer&Result_61::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( 0 );

	// Friends
	m_stream.write_u32(1);
	m_stream.write_utf16(L"String_1");

	m_stream.write_u32(1);
	m_stream.write_utf16(L"String_2");

	// Ignore
	m_stream.write_u32(1);
	m_stream.write_utf16(L"String_3");

	return m_stream;
}
