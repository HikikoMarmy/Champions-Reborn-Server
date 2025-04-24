#include "../../global_define.h"
#include "Request_5B.h"

void Request_5B::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response Request_5B::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto publicKey = stream->read_utf8();
	auto unknown = stream->read_u32();

	return std::make_shared< Result_5B >( this );
}

Result_5B::Result_5B( GenericRequest *request ) : GenericResponse( *request )
{
}

ByteStream&Result_5B::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	return m_stream;
}
