#include "../../global_define.h"
#include "RequestUpdateGameData.h"

void RequestUpdateGameData::Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_sessionId = stream->read_encrypted_utf16();
	m_gameData = stream->read_utf8();
	//m_hardwareIp = stream->read_sz_utf8();
}

sptr_generic_response RequestUpdateGameData::ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	Deserialize( socket, stream );

	Log::Debug( "RequestUpdateGameData : %S", m_sessionId.c_str() );

	Log::Packet( stream->data, stream->data.size(), false );

	return std::make_shared< ResultUpdateGameData >( this );
}

ResultUpdateGameData::ResultUpdateGameData( GenericRequest *request ) : GenericResponse( *request )
{
	
}

ByteStream &ResultUpdateGameData::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	return m_stream;
}
