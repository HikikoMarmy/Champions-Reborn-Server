#include "../../global_define.h"
#include "RequestSaveNewCharacter_RTA.h"

void RequestSaveNewCharacter::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestSaveNewCharacter::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	return std::make_shared< ResultSaveNewCharacter >( this );
}

ResultSaveNewCharacter::ResultSaveNewCharacter( GenericRequest *request ) : GenericResponse( *request )
{
}

ByteStream&ResultSaveNewCharacter::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	return m_stream;
}
