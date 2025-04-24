#include "../../global_define.h"
#include "RequestGetPublicRooms.h"

void RequestGetPublicRooms::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestGetPublicRooms::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto publicKey = stream->read_utf8();

	return std::make_shared< ResultGetPublicRooms >( this );
}

ResultGetPublicRooms::ResultGetPublicRooms( GenericRequest *request ) : GenericResponse( *request )
{
}

ByteStream& ResultGetPublicRooms::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	m_stream.write_u32( 0);
	for( int i = 0; i < 0; i++ )
	{
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
	}

	return m_stream;
}
