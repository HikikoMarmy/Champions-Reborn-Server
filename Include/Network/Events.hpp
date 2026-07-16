#pragma once

#include <map>
#include <functional>

/* 0001 */	#include "Event/RequestAddFriend.hpp"
/* 0002 */	#include "Event/RequestAddIgnore.hpp"

/* 0005 */	#include "Event/RequestCancelGame.hpp"
/* 0006 */	#include "Event/RequestCreateAccount.hpp"

/* 0008 */	#include "Event/RequestCreatePrivateGame.hpp"
/* 0009 */	#include "Event/RequestCreatePrivateRoom.hpp"

/* 000A */	#include "Event/RequestCreatePublicGame.hpp"

/* 000C */	#include "Event/RequestEnterRoom.hpp"
/* 000D */	#include "Event/RequestGetGame.hpp"
/* 000E */	#include "Event/RequestGetPublicRooms.hpp"
/* 000F */	#include "Event/RequestGetRealmStats.hpp"

/* 0011 */	#include "Event/RequestGetRoom.hpp"
/* 0015 */	#include "Event/RequestLeaveRoom.hpp"
/* 0016 */	#include "Event/RequestLogin.hpp"
/* 0017 */	#include "Event/RequestLogout.hpp"
/* 0018 */	#include "Event/RequestMatchGame.hpp"

/* 001C */	#include "Event/RequestRemoveFriend.hpp"
/* 001D */	#include "Event/RequestRemoveIgnore.hpp"

/* 0021 */	#include "Event/RequestSendInstantMessage.hpp"
/* 0022 */	#include "Event/RequestSendRoomMessage.hpp"
/* 0023 */	#include "Event/RequestStartGame.hpp"
/* 0024 */	#include "Event/RequestTouchSession.hpp"
/* 0025 */	#include "Event/RequestDoClientDiscovery.hpp"

/* 0027 */	#include "Event/RequestGetEncryptionKey.hpp"

/* 0041 */	#include "Event/NotifyForcedLogout.hpp"
/* 0042 */	#include "Event/RequestGetRules.hpp"
/* 0043 */	#include "Event/RequestGetServerAddress.hpp"
/* 0044 */	#include "Event/RequestUpdateGameData.hpp"

/* 0054 */	#include "Event/RequestCreatePublicGame_RTA.hpp"
/* 0055 */	#include "Event/RequestMatchGame_RTA.hpp"
/* 0056 */	#include "Event/RequestCreatePrivateGame_RTA.hpp"
/* 0057 */	#include "Event/RequestGetGame_RTA.hpp"

/* 0058 */	#include "Event/RequestCreateNewCharacter_RTA.hpp"
/* 005A */	#include "Event/RequestDeleteCharacter_RTA.hpp"
/* 005B */	#include "Event/RequestGetNetCharacterList_RTA.hpp"
/* 005C */	#include "Event/RequestGetCharacterData_RTA.hpp"
/* 005D */	#include "Event/RequestAppendCharacterData.hpp"
/* 005E */	#include "Event/RequestSaveCharacter_RTA.hpp"
/* 005F */	#include "Event/RequestUserJoinSuccess.hpp"
/* 0060 */	#include "Event/RequestCancelGame_RTA.hpp"
/* 0061 */	#include "Event/RequestGetSocialListInitial.hpp"
/* 0062 */	#include "Event/RequestGetSocialListUpdate.hpp"

/* 0066 */	#include "Event/RequestDoClientDiscovery_RTA.hpp"


const std::map< int16_t, std::function< std::unique_ptr< GenericRequest >() > > REQUEST_EVENT =
{
	{ 0x0001, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestAddFriend >();
		}
	},
	{ 0x0002, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestAddIgnore >();
		}
	},
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
	{ 0x0015, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestLeaveRoom >();
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
	{ 0x001C, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestRemoveFriend >();
		}
	},
	{ 0x001D, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestRemoveIgnore >();
		}
	},
	{ 0x0021, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestSendInstantMessage >();
		}
	},
	{ 0x0022, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestSendRoomMessage >();
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
	{ 0x0057, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestGetGame_RTA >();
		}
	},
	{ 0x0058, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestCreateNewCharacter_RTA >();
		}
	},
	{ 0x005A, []() -> std::unique_ptr< GenericRequest >
	{
		return std::make_unique< RequestDeleteCharacter_RTA >();
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
			return std::make_unique< RequestAppendCharacterData >();
		}
	},
	{ 0x005E, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestSaveCharacter_RTA >();
		}
	},
	{ 0x005F, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestUserJoinSuccess >();
		}
	},
	{ 0x0060, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestCancelGame_RTA >();
		}
	},
	{ 0x0061, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestGetSocialListInitial >();
		}
	},
	{ 0x0062, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestGetSocialListUpdate >();
		}
	},
	{ 0x0066, []() -> std::unique_ptr< GenericRequest >
		{
			return std::make_unique< RequestDoClientDiscovery_RTA >();
		}
	},
};