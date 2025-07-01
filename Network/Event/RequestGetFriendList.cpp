#include "RequestGetFriendList.h"

void RequestGetFriendList::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestGetFriendList::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto publicKey = stream->read_utf8();
	auto unknown = stream->read_u32();

	return std::make_shared< ResultGetFriendList >( this );
}

ResultGetFriendList::ResultGetFriendList( GenericRequest *request ) : GenericResponse( *request )
{
}

void ResultGetFriendList::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( 0 );

	// Friends
	out.write_u32(1);
	out.write_utf16(L"String_1");

	out.write_u32(1);
	out.write_utf16(L"String_2");

	// Ignore
	out.write_u32(1);
	out.write_utf16(L"String_3");
}
