#include "app/net/tcpObject.h"
#include "platform/threads/thread.h"

#ifndef _LOBBY_H_
#define _LOBBY_H_

class LobbyServerConnection : public TCPObject
{
private:
   typedef TCPObject Parent;
   String readString(U8* buffer, U32 bufferLen);

   enum SEND_MESSAGE_ACTION
   {
      CLIENT_ERROR = 1,
      LIST_LOBBIES = 2,
      CREATE_LOBBY = 3,
      JOIN_LOBBY = 4,
      SEND_MESSAGE = 5,
   };

   enum RECEIVED_MESSAGE_ACTION
   {
      SERVER_ERROR = 1,
      LOBBY_LIST = 2,
      LOBBY_CREATED = 3,
      LOBBY_MESSAGE = 4,
   };

public:
   LobbyServerConnection() {  }

   DECLARE_CONOBJECT(LobbyServerConnection);
   void parseLobbyList(U8* buffer, U32 bufferLen);
   U32 onReceive(U8* buffer, U32 bufferLen) override;

   void updateLobbyList();
   void createLobby(const char* name);
   void joinLobby(const char* id);
   void sendMessage(const char* message);
};

class LobbyServer : public SimObject
{
private:
   typedef SimObject Parent;

public:
   LobbyServer();
   ~LobbyServer() override;

   DECLARE_CONOBJECT(LobbyServer);

   bool onAdd() override;

private:
   const char* mLobbyId;
   StringTableEntry mAddress;
   LobbyServerConnection* mLobbyServerConnection;
};

#endif
