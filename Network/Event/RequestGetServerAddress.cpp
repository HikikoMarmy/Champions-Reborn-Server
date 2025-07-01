#include "RequestGetServerAddress.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/RealmUser.h"
#include "../../Common/Constant.h"
#include "../../configuration.h"
#include "../../logging.h"

void RequestGetServerAddress::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );
}

sptr_generic_response RequestGetServerAddress::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	if( socket->gameType == RealmGameType::RETURN_TO_ARMS )
	{
		return std::make_shared< ResultGetServerAddress >( this, Config::service_ip, Config::rta_lobby_port, socket->gameType );
	}
	else
	{
		return std::make_shared< ResultGetServerAddress >( this, Config::service_ip, Config::con_lobby_port, socket->gameType );
	}
}

ResultGetServerAddress::ResultGetServerAddress( GenericRequest *request, std::string ip, int32_t port, RealmGameType gameType ) : GenericResponse( *request )
{
	m_ip = ip;
	m_port = port;
	m_gameType = gameType;
}

void ResultGetServerAddress::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( 0 );

	if( m_gameType == RealmGameType::RETURN_TO_ARMS )
		out.write_utf8( m_ip );
	else
		out.write_sz_utf8( m_ip );

	out.write( m_port );
}
