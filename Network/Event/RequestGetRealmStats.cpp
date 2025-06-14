#include "RequestGetRealmStats.h"

#include "../../Game/RealmUserManager.h"

void RequestGetRealmStats::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestGetRealmStats::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	return std::make_shared< ResultGetRealmStats >( this );
}

ResultGetRealmStats::ResultGetRealmStats( GenericRequest *request ) : GenericResponse( *request )
{
	
}

ByteBuffer &ResultGetRealmStats::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( 0 );

	// Player count on the game page.
	m_stream.write_u32( RealmUserManager::Get().GetUserCount() );	// Users Logged In Game

	// I'm not sure this appears anywhere in the game.
	m_stream.write_u32( 0 );	// Users Logged In Realm
	m_stream.write_u32( 0 );	// Users Running Game
	m_stream.write_u32( 0 );	// Users Running Realm
	m_stream.write_u32( 0 );	// Users Playing Game
	m_stream.write_u32( 0 );	// Users Playing Realm
	m_stream.write_u32( 0 );	// unmatchedGamesGame
	m_stream.write_u32( 0 );	// unmatchedGamesRealm
	return m_stream;
}
