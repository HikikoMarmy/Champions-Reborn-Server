#include "RequestCreateAccount.h"

#include "../../Game/RealmUserManager.h"
#include "../../Crypto/PasswordHash.h"
#include "../../Common/Constant.h"
#include "../../Game/RealmUser.h"
#include "../../Database/Database.h"
#include "../../logging.h"

void RequestCreateAccount::Deserialize( sptr_byte_stream stream )
{
	DeserializeHeader( stream );

	m_username = stream->read_encrypted_utf16();
	m_password = stream->read_encrypted_utf16();
	m_emailAddress = stream->read_encrypted_utf16();
	m_dateOfBirth = stream->read_encrypted_utf16();
	m_chatHandle = stream->read_encrypted_utf16();
}

bool RequestCreateAccount::VerifyUserData()
{
	if( m_username.empty() || m_password.empty() || m_emailAddress.empty() || m_dateOfBirth.empty() || m_chatHandle.empty() )
	{
		Log::Error( "RequestCreateAccount::VerifyUserData() - Missing required fields for account creation." );
		return false;
	}

	if( m_username.length() < 3 || m_username.length() > 20 )
	{
		Log::Error( "RequestCreateAccount::VerifyUserData() - Username must be between 3 and 20 characters." );
		return false;
	}

	if( m_password.length() < 6 || m_password.length() > 32 )
	{
		Log::Error( "RequestCreateAccount::VerifyUserData() - Password must be between 6 and 32 characters." );
		return false;
	}

	return true;
}

sptr_generic_response RequestCreateAccount::ProcessRequest( sptr_socket socket, sptr_byte_stream stream )
{
	Deserialize( stream );

	auto user = UserManager::Get().FindUserBySocket( socket );
	if( nullptr == user || user->m_gameType != RealmGameType::RETURN_TO_ARMS )
	{
		return std::make_shared< ResultCreateAccount >( this, ERROR_FATAL, L"" );
	}

	if( m_username.empty() || m_password.empty() || m_emailAddress.empty() || m_dateOfBirth.empty() || m_chatHandle.empty() )
	{
		Log::Error( "RequestCreateAccount::ProcessRequest() - Missing required fields for account creation." );
		return std::make_shared< ResultCreateAccount >( this, ERROR_FATAL, L"" );
	}

	auto result = Database::Get().CreateNewAccount
	(
		Util::WideToUTF8( m_username ),
		Util::WideToUTF8( m_password ),
		Util::WideToUTF8( m_emailAddress ),
		Util::WideToUTF8( m_dateOfBirth ),
		Util::WideToUTF8( m_chatHandle )
	);

	if( !result )
	{
		Log::Error( "RequestCreateAccount::ProcessRequest() - Failed to create account for user: {}", m_username );
		return std::make_shared< ResultCreateAccount >( this, ERROR_FATAL, L"" );
	}

	user->m_isLoggedIn = true;
	user->m_sessionId = UserManager::Get().GenerateSessionId();
	user->m_accountId = result;
	user->m_username = m_username;
	user->m_chatHandle = m_chatHandle;

	return std::make_shared< ResultCreateAccount >( this, SUCCESS, user->m_sessionId );
}

ResultCreateAccount::ResultCreateAccount( GenericRequest *request, int32_t reply, std::wstring sessionId ) : GenericResponse( *request )
{
	m_reply = reply;
	m_sessionId = sessionId;
}

void ResultCreateAccount::Serialize( ByteBuffer &out ) const
{
	out.write_u16( m_packetId );
	out.write_u32( m_trackId );
	out.write_u32( m_reply );

	out.write_encrypted_utf16( m_sessionId );
}
