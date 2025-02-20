#pragma once

#include <map>
#include <functional>

#include "Event/RequestCancelGame.h"
#include "Event/RequestCreateAccount.h"
#include "Event/RequestCreatePrivateGame.h"
#include "Event/RequestCreatePublicGame.h"
#include "Event/RequestGetGame.h"
#include "Event/RequestGetRealmStats.h"
#include "Event/RequestLogin.h"
#include "Event/RequestLogout.h"
#include "Event/RequestMatchGame.h"
#include "Event/RequestTouchSession.h"
#include "Event/RequestDoClientDiscovery.h"
#include "Event/RequestGetEncryptionKey.h"
#include "Event/RequestGetRules.h"
#include "Event/RequestUpdateGameData.h"

const std::map< int16_t, std::function< std::unique_ptr< GenericRequest >() > > LOBBY_REQUEST_EVENT =
{
	{ 0x0005, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestCancelGame >();
		}
	},
	{ 0x0006, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestCreateAccount >();
		}
	},
	{ 0x0008, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestCreatePrivateGame >();
		}
	},
	{ 0x000A, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestCreatePublicGame >();
		}
	},
	{ 0x000D, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestGetGame >();
		}
	},
	{ 0x000F, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestGetRealmStats >();
		}
	},
	{ 0x0016, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestLogin >();
		}
	},
	{ 0x0017, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestLogout >();
		}
	},
	{ 0x0018, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestMatchGame >();
		}
	},
	{ 0x0024, []() -> std::unique_ptr< GenericRequest >
		{
		return std::make_unique< RequestTouchSession >();
		}
	},
	{ 0x0025, []() -> std::unique_ptr< GenericRequest >
		{
		return std::make_unique< RequestDoClientDiscovery >();
		}
	},
	{ 0x0027, []() -> std::unique_ptr< GenericRequest >
		{
		return std::make_unique< RequestGetEncryptionKey >();
		}
	},
	{ 0x0042, []() -> std::unique_ptr< GenericRequest >
		{
		return std::make_unique< RequestGetRules >();
		}
	},
	{
	 0x0044, []() -> std::unique_ptr< GenericRequest >
	  {
	  return std::make_unique< RequestUpdateGameData >();
	  }
	}

};