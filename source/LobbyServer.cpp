#include "LobbyServer.h"

#include "console/engineAPI.h"
#include "platform/threads/thread.h"

IMPLEMENT_CONOBJECT(LobbyServer);
IMPLEMENT_CONOBJECT(LobbyServerConnection);

LobbyServer::LobbyServer()
{
}

LobbyServer::~LobbyServer()
{
}

bool LobbyServer::onAdd()
{
   if (String::isEmpty(mAddress))
   {
      Con::errorf("LobbyServer must be given an address");
      return false;
   }

   mLobbyServerConnection = new LobbyServerConnection();
   AssertFatal(mLobbyServerConnection->registerObject(), "Lobby TCP connection object");

   mLobbyServerConnection->connect(mAddress);

   return true;
}

U32 LobbyServerConnection::onReceive(U8* buffer, U32 bufferLen)
{
   S32 endIdx = -1;
   for (S32 idx = 0; idx < bufferLen; idx++)
   {
      if (buffer[idx] == '\t')
      {
         endIdx = idx;
      }
   }
   if (endIdx == -1)
   {
      return 0;
   }

   // Remove message type byte
   U8* msgBuffer = buffer + 1;
   // Subtract message type byte and line separator
   U32 msgLen = endIdx - 1;

   switch (buffer[0])
   {
   case LOBBY_LIST:
      Con::printf("Received a lobby list of length %d", msgLen);
      parseLobbyList(msgBuffer, msgLen);
      break;
   case LOBBY_CREATED:
      Con::printf("Received a lobby created event of length %d", msgLen);
      break;
   case LOBBY_MESSAGE:
      Con::printf("Received a lobby message of length %d", msgLen);
      break;
   case SERVER_ERROR:
      Con::warnf("Received a server error of length %d", msgLen);
      break;
   default:
      Con::errorf("Received unknown message type from lobby server");
      return 0;
   }

   return endIdx + 1;
}

void LobbyServerConnection::updateLobbyList()
{
   const U8 mBuffer[2] = { LIST_LOBBIES, '\t' };
   send(mBuffer, 2);
}

void LobbyServerConnection::createLobby(const char* name)
{
   Con::errorf("LobbyServerConnection::createLobby not implemented");
}

void LobbyServerConnection::joinLobby(const char* id)
{
   Con::errorf("LobbyServerConnection::joinLobby not implemented");
}

void LobbyServerConnection::sendMessage(const char* message)
{
   Con::errorf("LobbyServerConnection::sendMessage not implemented");
}

void LobbyServerConnection::parseLobbyList(U8* buffer, U32 bufferLen)
{
   if (bufferLen == 0)
   {
      Con::printf("No Lobbies Registered");
      return;
   }

   U32 idx = 0;
   while (idx < bufferLen)
   {
      String lobbyId = readString(buffer + idx, bufferLen - idx);
      idx += lobbyId.length() + 1;
      String lobbyName = readString(buffer + idx, bufferLen - idx);
      idx += lobbyName.length() + 1;
      // Discard timestamp for now
      idx += 15;
      U32 subscribers = buffer[idx];
      // 4*8 = 32
      idx += 4;
      Con::printf("Lobby %s (%s): %d", lobbyId.c_str(), lobbyName.c_str(), subscribers);
   }
}

String LobbyServerConnection::readString(U8* buffer, U32 bufferLen)
{
   U8 length = buffer[0];
   const char* strStart = (const char*)buffer + 1;
   const char* strEnd = (const char*)(strStart + length);
   return String::SpanToString(strStart, strEnd);
}

DefineEngineMethod(LobbyServerConnection, updateLobbyList, void, (),,"")
{
   object->updateLobbyList();
}
