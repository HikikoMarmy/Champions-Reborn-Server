#include "RequestGetRules.h"

#include "../../Game/RealmUserManager.h"
#include "../../Game/RealmUser.h"
#include "../../Common/Constant.h"
#include "../../logging.h"

void RequestGetRules::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_language = stream->read_sz_utf8();
}

sptr_generic_response RequestGetRules::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = RealmUserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		return std::make_shared< ResultGetRules >( this, L"" );
	}

	// TODO: Get rules/eula based on language
	// and move it info a MOTD file.
	std::wstring rules;
	
	if (user->m_gameType == RealmGameType::RETURN_TO_ARMS)
	{
		rules = L"Welcome to the Norrath Emulated Server!\n\n"
			L"RETURN TO ARMS network support is currently a\n"
			L"work in progress and can not guarantee stability.\n\n"
			L"[IMPORTANT]:\n"
			L"Please note that ONLINE character data will not be saved.\n"
			L"You must import an offline save file for now.\n";
	}
	else
	{
		rules = L"Welcome to the Norrath Emulated Server!\n\n"
			L"This server is currently in development\n"
			L"and may not be fully functional.\n\n";
	}

	 return std::make_shared< ResultGetRules >( this, rules );
}

ResultGetRules::ResultGetRules( GenericRequest *request, std::wstring rules ) : GenericResponse( *request )
{
	m_rules = rules;
}

ByteBuffer &ResultGetRules::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( 0 );

	m_stream.write_utf16( m_rules );

	return m_stream;
}
