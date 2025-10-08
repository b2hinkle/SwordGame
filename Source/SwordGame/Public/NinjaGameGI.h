// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "NinjaGameGI.generated.h"

/**
 * 
 */
UCLASS()
class SWORDGAME_API UNinjaGameGI : public UGameInstance/*, public ISteamMatchmakingPingResponse, public ISteamMatchmakingServers*/
{
	GENERATED_BODY()
	
public:

	void Init() override;

public:
	//void JoinServer(uint32 IP, uint32 Port);

#pragma region ISteamMatchmakingServers Pure Implentations
	//virtual HServerListRequest RequestInternetServerList(AppId_t iApp, ARRAY_COUNT(nFilters)MatchMakingKeyValuePair_t** ppchFilters, uint32 nFilters, ISteamMatchmakingServerListResponse* pRequestServersResponse) override;
	//virtual HServerListRequest RequestLANServerList(AppId_t iApp, ISteamMatchmakingServerListResponse* pRequestServersResponse) override;
	//virtual HServerListRequest RequestFriendsServerList(AppId_t iApp, ARRAY_COUNT(nFilters)MatchMakingKeyValuePair_t** ppchFilters, uint32 nFilters, ISteamMatchmakingServerListResponse* pRequestServersResponse) override;
	//virtual HServerListRequest RequestFavoritesServerList(AppId_t iApp, ARRAY_COUNT(nFilters)MatchMakingKeyValuePair_t** ppchFilters, uint32 nFilters, ISteamMatchmakingServerListResponse* pRequestServersResponse) override;
	//virtual HServerListRequest RequestHistoryServerList(AppId_t iApp, ARRAY_COUNT(nFilters)MatchMakingKeyValuePair_t** ppchFilters, uint32 nFilters, ISteamMatchmakingServerListResponse* pRequestServersResponse) override;
	//virtual HServerListRequest RequestSpectatorServerList(AppId_t iApp, ARRAY_COUNT(nFilters)MatchMakingKeyValuePair_t** ppchFilters, uint32 nFilters, ISteamMatchmakingServerListResponse* pRequestServersResponse) override;
	//virtual void ReleaseRequest(HServerListRequest hServerListRequest) override;
	//virtual gameserveritem_t* GetServerDetails(HServerListRequest hRequest, int iServer) override;
	//virtual void CancelQuery(HServerListRequest hRequest) override;
	//virtual void RefreshQuery(HServerListRequest hRequest) override;
	//virtual bool IsRefreshing(HServerListRequest hRequest) override;
	//virtual int GetServerCount(HServerListRequest hRequest) override;
	//virtual void RefreshServer(HServerListRequest hRequest, int iServer) override;
	//virtual HServerQuery PingServer(uint32 unIP, uint16 usPort, ISteamMatchmakingPingResponse* pRequestServersResponse) override;
	//virtual HServerQuery PlayerDetails(uint32 unIP, uint16 usPort, ISteamMatchmakingPlayersResponse* pRequestServersResponse) override;
	//virtual HServerQuery ServerRules(uint32 unIP, uint16 usPort, ISteamMatchmakingRulesResponse* pRequestServersResponse) override;
	//virtual void CancelServerQuery(HServerQuery hServerQuery) override;
#pragma endregion

protected:
//Overrides
	//#pragma region ISteamMatchmakingPingResponse pure Implementations
	//virtual void ServerResponded(gameserveritem_t& server) override;
	//virtual void ServerFailedToRespond() override;
	//#pragma endregion


	
};
