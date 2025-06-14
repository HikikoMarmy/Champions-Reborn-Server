#include "RequestLogin.h"

#include "../../Database/Database.h"
#include "../../Game/RealmUserManager.h"
#include "../../Game/RealmUser.h"
#include "../../Common/Constant.h"
#include "../../logging.h"

void RequestLogin::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_username =  Util::WideToUTF8( stream->read_encrypted_utf16() );
	m_password =  Util::WideToUTF8( stream->read_encrypted_utf16() );
}

sptr_generic_response RequestLogin::ProcessLoginCON(sptr_user user)
{
	if (m_username != "foo" && m_password != "bar")
	{
		// Network Beta CoN uses login information, but it's invalid because of version 2.0
		// which used "foo" and "bar" as the login credentials.
		Log::Debug("RequestLogin : Champions of Norrath v1.0");

		// TODO: Either block this, or add support for the network beta.
		return std::make_shared< ResultLogin >(this, LOGIN_REPLY::ACCOUNT_INVALID, L"");
	}

	user->m_accountId = -1;
	user->m_sessionId = RealmUserManager::Get().GenerateSessionId();

	return std::make_shared< ResultLogin >(this, SUCCESS, user->m_sessionId);
}

sptr_generic_response RequestLogin::ProcessLoginRTA(sptr_user user)
{
	// Return to Arms uses login information.
	Log::Debug("RequestLogin : Return to Arms");

	// Verify the account exists
	auto accountId = Database::Get().VerifyAccount(m_username, m_password);

	if (accountId < 0)
	{
		Log::Error("RequestLogin::ProcessRequest() - Invalid account ID: " + std::to_string(accountId));
		return std::make_shared< ResultLogin >(this, ACCOUNT_INVALID, L"");
	}

	user->m_accountId = accountId;
	user->m_sessionId = RealmUserManager::Get().GenerateSessionId();

	Database::Get().CreateSession(
		user->m_accountId,
		user->m_sessionId,
		user->sock->remote_ip);

	return std::make_shared< ResultLogin >(this, SUCCESS, user->m_sessionId);
}

sptr_generic_response RequestLogin::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = RealmUserManager::Get().FindUserBySocket( socket );
	if( user == nullptr )
	{
		Log::Error( "RequestLogin::ProcessRequest() - User not found" );
		return std::make_shared< ResultLogin >( this, ACCOUNT_INVALID, L"" );
	}

	if( m_username.empty() || m_password.empty() )
	{
		Log::Error( "RequestLogin::ProcessRequest() - Username or password is empty" );
		return std::make_shared< ResultLogin >( this, ACCOUNT_INVALID, L"" );
	}

	if( user->m_gameType == RealmGameType::CHAMPIONS_OF_NORRATH )
	{
		return ProcessLoginCON( user );
	}
	else
	{
		return ProcessLoginRTA( user );
	}
}

ResultLogin::ResultLogin( GenericRequest *request, int32_t reply, std::wstring sessionId ) : GenericResponse( *request )
{
	m_reply = reply;
	m_sessionId = sessionId;
}

ByteBuffer &ResultLogin::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( m_reply );

	m_stream.write_encrypted_utf16( m_sessionId );
	m_stream.write_encrypted_utf16( L"UNKNOWN DUMMY STRING" );

	return m_stream;
}
