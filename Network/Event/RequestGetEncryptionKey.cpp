#include "../../global_define.h"
#include "RequestGetEncryptionKey.h"

void RequestGetEncryptionKey::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestGetEncryptionKey::ProcessRequest( sptr_user user, sptr_byte_stream stream )
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

ByteStream& ResultGetEncryptionKey::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	auto encrypted = RealmCrypt::encryptSymmetric( m_symKey );

	m_stream.write_u32( encrypted.size() + 4 );
	m_stream.write_u32( m_symKey.size() );
	m_stream.write_bytes( encrypted );

	return m_stream;
}
