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

void ResultGetRealmStats::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( 0 );

	// Player count on the game page.
	out.write_u32( UserManager::Get().GetUserCount() );	// Users Logged In Game

	// I'm not sure this appears anywhere in the game.
	out.write_u32( 0 );	// Users Logged In Realm
	out.write_u32( 0 );	// Users Running Game
	out.write_u32( 0 );	// Users Running Realm
	out.write_u32( 0 );	// Users Playing Game
	out.write_u32( 0 );	// Users Playing Realm
	out.write_u32( 0 );	// unmatchedGamesGame
	out.write_u32( 0 );	// unmatchedGamesRealm
}
