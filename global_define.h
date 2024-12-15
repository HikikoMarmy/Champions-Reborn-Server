
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define FD_SETSIZE 1024
#include <WinSock2.h>

#include <Windows.h>
#include <cstdint>

#include "misc/math.h"
#include "misc/Timer.h"

#include "misc/threadsafe_queue.hpp"
#include "misc/Encryptor.h"
#include "misc/ByteStream.h"

#include "ui/logging.h"
#include "configuration.h"

#include "network/socket.h"
#include "network/packet.h"
#include "network/socket_manager.h"

#include "game/client.h"
#include "game/client_manager.h"
#include "network/protocol_gateway.h"
#include "network/protocol_game.h"

extern CLogManager							logging;
extern std::unique_ptr< CTimer >			server_time;
extern std::unique_ptr< CClientManager >	client_manager;
extern std::unique_ptr< CSocketManager >	socket_manager;

void Initialize_Global();