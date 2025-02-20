#include "../../global_define.h"
#include "RequestGetRealmStats.h"

void RequestGetRealmStats::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestGetRealmStats::ProcessRequest( sptr_user user, sptr_byte_stream stream )
{
	Deserialize( stream );

	return std::make_shared< ResultGetRealmStats >( this );
}

ResultGetRealmStats::ResultGetRealmStats( GenericRequest *request ) : GenericResponse( *request )
{
	
}

ByteStream &ResultGetRealmStats::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_requestId );
	m_stream.write_u32( 0 );

	m_stream.write_u32( 1 );	// Users Logged In Game
	m_stream.write_u32( 2 );	// Users Logged In Realm
	m_stream.write_u32( 3 );	// Users Running Game
	m_stream.write_u32( 4 );	// Users Running Realm
	m_stream.write_u32( 5 );	// Users Playing Game
	m_stream.write_u32( 6 );	// Users Playing Realm
	m_stream.write_u32( 7 );	// unmatchedGamesGame
	m_stream.write_u32( 8 );	// unmatchedGamesRealm
	return m_stream;
}
