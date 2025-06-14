#pragma once

#include <map>
#include <functional>

/* 0005 */	#include "Event/RequestCancelGame.h"
/* 0006 */	#include "Event/RequestCreateAccount.h"

/* 0008 */	#include "Event/RequestCreatePrivateGame.h"
/* 0009 */	#include "Event/RequestCreatePrivateRoom.h"

/* 000A */	#include "Event/RequestCreatePublicGame.h"

/* 000C */	#include "Event/RequestEnterRoom.h"
/* 000D */	#include "Event/RequestGetGame.h"
/* 000E */	#include "Event/RequestGetPublicRooms.h"
/* 000F */	#include "Event/RequestGetRealmStats.h"

/* 0011 */	#include "Event/RequestGetRoom.h"

/* 0016 */	#include "Event/RequestLogin.h"
/* 0017 */	#include "Event/RequestLogout.h"
/* 0018 */	#include "Event/RequestMatchGame.h"

/* 0023 */	#include "Event/RequestStartGame.h"
/* 0024 */	#include "Event/RequestTouchSession.h"
/* 0025 */	#include "Event/RequestDoClientDiscovery.h"

/* 0027 */	#include "Event/RequestGetEncryptionKey.h"

/* 0042 */	#include "Event/RequestGetRules.h"
/* 0043 */	#include "Event/RequestGetServerAddress.h"
/* 0044 */	#include "Event/RequestUpdateGameData.h"

/* 0054 */	#include "Event/RequestCreatePublicGame_RTA.h"
/* 0055 */	#include "Event/RequestMatchGame_RTA.h"
/* 0056 */	#include "Event/RequestCreatePrivateGame_RTA.h"

/* 0058 */	#include "Event/RequestCreateNewCharacter_RTA.h"
/* 005B */	#include "Event/RequestGetNetCharacterList_RTA.h"
/* 005C */	#include "Event/RequestGetCharacterData_RTA.h"
/* 005D */	#include "Event/Request_5D.h"
/* 005E */	#include "Event/RequestSaveCharacter_RTA.h"
/* 005F */	#include "Event/Request_5F.h"
/* 0060 */	#include "Event/RequestCancelGame_RTA.h"
/* 0061 */	#include "Event/Request_61.h"

/* 0066 */	#include "Event/RequestGetGame_RTA.h"


const std::map< int16_t, std::function< std::unique_ptr< GenericRequest >() > > REQUEST_EVENT =
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
	{ 0x0009, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestCreatePrivateRoom >();
		}
	},
	{ 0x000A, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestCreatePublicGame >();
		}
	},
	{ 0x000C, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestEnterRoom >();
		}
	},
	{ 0x000D, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestGetGame >();
		}
	},
	{ 0x000E, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestGetPublicRooms >();
		}
	},
	{ 0x000F, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestGetRealmStats >();
		}
	},
	{ 0x0011, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestGetRoom >();
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
	{ 0x0023, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestStartGame >();
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
	{ 0x0043, []() -> std::unique_ptr< GenericRequest >
		{
		return std::make_unique< RequestGetServerAddress >();
		}
	},
	{ 0x0044, []() -> std::unique_ptr< GenericRequest >
	  {
	  return std::make_unique< RequestUpdateGameData >();
	  }
	},
	{ 0x0054, []() -> std::unique_ptr< GenericRequest >
	  {
	  return std::make_unique< RequestCreatePublicGame_RTA >();
	  }
	},
	{ 0x0055, []() -> std::unique_ptr< GenericRequest >
	  {
	  return std::make_unique< RequestMatchGame_RTA >();
	  }
	},
	{ 0x0056, []() -> std::unique_ptr< GenericRequest >
	  {
	  return std::make_unique< RequestCreatePrivateGame_RTA >();
	  }
	},
	{ 0x0058, []() -> std::unique_ptr< GenericRequest >
	  {
	  return std::make_unique< RequestCreateNewCharacter_RTA >();
	  }
	},
	{ 0x005B, []() -> std::unique_ptr< GenericRequest >
	  {
	  return std::make_unique< RequestGetNetCharacterList_RTA >();
	  }
	},
	{ 0x005C, []() -> std::unique_ptr< GenericRequest >
	  {
	  return std::make_unique< RequestGetNetCharacterData_RTA >();
	  }
	},
	{ 0x005D, []() -> std::unique_ptr< GenericRequest >
	  {
	  return std::make_unique< Request_5D >();
	  }
	},
	{ 0x005E, []() -> std::unique_ptr< GenericRequest >
	  {
	  return std::make_unique< RequestSaveCharacter_RTA >();
	  }
	},
	{ 0x005F, []() -> std::unique_ptr< GenericRequest >
	  {
	  return std::make_unique< Request_5F >();
	  }
	},
	{ 0x0060, []() -> std::unique_ptr< GenericRequest >
	  {
	  return std::make_unique< RequestCancelGame_RTA >();
	  }
	},
	{ 0x0061, []() -> std::unique_ptr< GenericRequest >
	  {
	  return std::make_unique< Request_61 >();
	  }
	},
	{ 0x0066, []() -> std::unique_ptr< GenericRequest >
	 {
		return std::make_unique< RequestGetGame_RTA >();
	 }
	},
};