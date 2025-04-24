#include "../../global_define.h"
#include "Request_5D.h"

void Request_5D::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response Request_5D::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	return std::make_shared< Result_5D >( this );
}

Result_5D::Result_5D( GenericRequest *request ) : GenericResponse( *request )
{
}

ByteStream&Result_5D::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	return m_stream;
}
