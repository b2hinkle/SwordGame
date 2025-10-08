// Fill out your copyright notice in the Description page of Project Settings.


#include "NinjaGameGI.h"

#include "PlayerMenuInputProcessor.h"


void UNinjaGameGI::Init()
{
    Super::Init();

    FSlateApplication::Get().RegisterInputPreProcessor(MakeShared<FPlayerMenuInputProcessor>());
}

//#include "Steamworks/Steamv139/sdk/public/steam/isteammatchmaking.h"

//void UNinjaGameGI::JoinServer(uint32 IP, uint32 Port)
//{
//	ISteamMatchmakingServers::PingServer(IP, Port, this);
//}
//
//#pragma region ISteamMatchmakingPingResponse pure Impementations
//void UNinjaGameGI::ServerResponded(gameserveritem_t& server)
//{
//	
//}
//
//void UNinjaGameGI::ServerFailedToRespond()
//{
//	
//}
//#pragma endregion

#pragma region ISteamMatchmakingServers pure implementations

//HServerListRequest UNinjaGameGI::RequestInternetServerList(AppId_t iApp, ARRAY_COUNT(nFilters)MatchMakingKeyValuePair_t** ppchFilters, uint32 nFilters, ISteamMatchmakingServerListResponse* pRequestServersResponse)
//{
//	return HServerListRequest();
//}
//
//HServerListRequest UNinjaGameGI::RequestLANServerList(AppId_t iApp, ISteamMatchmakingServerListResponse* pRequestServersResponse)
//{
//	return HServerListRequest();
//}
//
//HServerListRequest UNinjaGameGI::RequestFriendsServerList(AppId_t iApp, ARRAY_COUNT(nFilters)MatchMakingKeyValuePair_t** ppchFilters, uint32 nFilters, ISteamMatchmakingServerListResponse* pRequestServersResponse)
//{
//	return HServerListRequest();
//}
//
//HServerListRequest UNinjaGameGI::RequestFavoritesServerList(AppId_t iApp, ARRAY_COUNT(nFilters)MatchMakingKeyValuePair_t** ppchFilters, uint32 nFilters, ISteamMatchmakingServerListResponse* pRequestServersResponse)
//{
//	return HServerListRequest();
//}
//
//HServerListRequest UNinjaGameGI::RequestHistoryServerList(AppId_t iApp, ARRAY_COUNT(nFilters)MatchMakingKeyValuePair_t** ppchFilters, uint32 nFilters, ISteamMatchmakingServerListResponse* pRequestServersResponse)
//{
//	return HServerListRequest();
//}
//
//HServerListRequest UNinjaGameGI::RequestSpectatorServerList(AppId_t iApp, ARRAY_COUNT(nFilters)MatchMakingKeyValuePair_t** ppchFilters, uint32 nFilters, ISteamMatchmakingServerListResponse* pRequestServersResponse)
//{
//	return HServerListRequest();
//}
//
//void UNinjaGameGI::ReleaseRequest(HServerListRequest hServerListRequest)
//{
//}
//
//gameserveritem_t* UNinjaGameGI::GetServerDetails(HServerListRequest hRequest, int iServer)
//{
//	return nullptr;
//}
//
//void UNinjaGameGI::CancelQuery(HServerListRequest hRequest)
//{
//}
//
//void UNinjaGameGI::RefreshQuery(HServerListRequest hRequest)
//{
//}
//
//bool UNinjaGameGI::IsRefreshing(HServerListRequest hRequest)
//{
//	return false;
//}
//
//int UNinjaGameGI::GetServerCount(HServerListRequest hRequest)
//{
//	return 0;
//}
//
//void UNinjaGameGI::RefreshServer(HServerListRequest hRequest, int iServer)
//{
//}
//
//HServerQuery UNinjaGameGI::PingServer(uint32 unIP, uint16 usPort, ISteamMatchmakingPingResponse* pRequestServersResponse)
//{
//	return HServerQuery();
//}
//
//HServerQuery UNinjaGameGI::PlayerDetails(uint32 unIP, uint16 usPort, ISteamMatchmakingPlayersResponse* pRequestServersResponse)
//{
//	return HServerQuery();
//}
//
//HServerQuery UNinjaGameGI::ServerRules(uint32 unIP, uint16 usPort, ISteamMatchmakingRulesResponse* pRequestServersResponse)
//{
//	return HServerQuery();
//}
//
//void UNinjaGameGI::CancelServerQuery(HServerQuery hServerQuery)
//{
//}
#pragma endregion
