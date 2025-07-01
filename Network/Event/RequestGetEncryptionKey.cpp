#include "RequestGetEncryptionKey.h"

void RequestGetEncryptionKey::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestGetEncryptionKey::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto publicKey = stream->read_utf8();
	auto unknown = stream->read_u32();

	return std::make_shared< ResultGetEncryptionKey >( this );
}

ResultGetEncryptionKey::ResultGetEncryptionKey( GenericRequest *request ) : GenericResponse( *request )
{
	m_symKey = RealmCrypt::getSymmetricKey();
}

void ResultGetEncryptionKey::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( 0 );

	auto encrypted = RealmCrypt::encryptSymmetric( m_symKey );

	out.write_u32( encrypted.size() + 4 );
	out.write_u32( m_symKey.size() );
	out.write_bytes( encrypted );
}
