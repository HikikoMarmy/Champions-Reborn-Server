#include "../../global_define.h"
#include "RequestGetEncryptionKey.h"

void RequestGetEncryptionKey::Deserialize( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestGetEncryptionKey::ProcessRequest( sptr_tcp_socket socket, sptr_byte_stream stream )
{
	Deserialize( socket, stream );

	auto publicKey = stream->read_utf8();
	auto unknown = stream->read_u32();

	return std::make_shared< ResultGetEncryptionKey >( this );
}

ResultGetEncryptionKey::ResultGetEncryptionKey( GenericRequest *request ) : GenericResponse( *request )
{
	m_symKey = RealmCrypt::getSymmetricKey();
}

ByteStream& ResultGetEncryptionKey::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	m_stream.write_encrypted_bytes( m_symKey );

	return m_stream;
}
