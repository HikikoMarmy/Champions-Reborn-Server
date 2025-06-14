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

	m_username = Util::WideToUTF8( stream->read_encrypted_utf16() );
	m_password = Util::WideToUTF8( stream->read_encrypted_utf16() );
	m_emailAddress = Util::WideToUTF8( stream->read_encrypted_utf16() );
	m_dateOfBirth = Util::WideToUTF8( stream->read_encrypted_utf16() );
	m_chatHandle = Util::WideToUTF8( stream->read_encrypted_utf16() );
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

	auto user = RealmUserManager::Get().FindUserBySocket( socket );
	if( nullptr == user || user->m_gameType != RealmGameType::RETURN_TO_ARMS )
	{
		return std::make_shared< ResultCreateAccount >( this, CREATE_ACCOUNT_REPLY::ERROR_FATAL, L"" );
	}

	if( m_username.empty() || m_password.empty() || m_emailAddress.empty() || m_dateOfBirth.empty() || m_chatHandle.empty() )
	{
		Log::Error( "RequestCreateAccount::ProcessRequest() - Missing required fields for account creation." );
		return std::make_shared< ResultCreateAccount >( this, CREATE_ACCOUNT_REPLY::ERROR_FATAL, L"" );
	}

	auto result = Database::Get().CreateNewAccount
	(
		m_username,
		m_password,
		m_emailAddress,
		m_dateOfBirth,
		m_chatHandle
	);

	if( !result )
	{
		Log::Error( "RequestCreateAccount::ProcessRequest() - Failed to create account for user: %s", m_username.c_str() );
		return std::make_shared< ResultCreateAccount >( this, CREATE_ACCOUNT_REPLY::ERROR_FATAL, L"" );
	}

	return std::make_shared< ResultCreateAccount >( this, CREATE_ACCOUNT_REPLY::SUCCESS, user->m_sessionId );
}

ResultCreateAccount::ResultCreateAccount( GenericRequest *request, int32_t reply, std::wstring sessionId ) : GenericResponse( *request )
{
	m_reply = reply;
	m_sessionId = sessionId;
}

ByteBuffer &ResultCreateAccount::Serialize()
{
	m_stream.write_u16( m_packetId );
	m_stream.write_u32( m_trackId );
	m_stream.write_u32( m_reply );

	m_stream.write_encrypted_utf16( m_sessionId );

	return m_stream;
}
