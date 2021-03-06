﻿#include "stdafx.h"
#include "DataPool.h"
#include "CommonConvert.h"
#include "GameService.h"

SharedMemory<RoleDataObject>*		g_pRoleDataObjectPool		= NULL;
SharedMemory<GlobalDataObject>*		g_pGlobalDataObjectPool	= NULL;
SharedMemory<BagDataObject>*		g_pBagDataObjectPool		= NULL;
SharedMemory<CopyDataObject>*		g_pCopyDataObjectPool		= NULL;
SharedMemory<EquipDataObject>*		g_pEquipDataObjectPool		= NULL;
SharedMemory<PetDataObject>*		g_pPetDataObjectPool		= NULL;
SharedMemory<PartnerDataObject>*	g_pPartnerDataObjectPool	= NULL;
SharedMemory<GuildDataObject>*		g_pGuildDataObjectPool		= NULL;
SharedMemory<MemberDataObject>*		g_pMemberDataObjectPool		= NULL;
SharedMemory<TaskDataObject>*		g_pTaskDataObjectPool		= NULL;
SharedMemory<MountDataObject>*		g_pMountDataObjectPool		= NULL;
SharedMemory<MailDataObject>*		g_pMailDataObjectPool		= NULL;
SharedMemory<GroupMailDataObject>*	g_pGroupMailDataObjectPool	= NULL;
SharedMemory<ActivityDataObject>*	g_pActivityDataObjectPool	= NULL;
SharedMemory<CounterDataObject>*	g_pCounterDataObjectPool	= NULL;
SharedMemory<FriendDataObject>*		g_pFriendDataObjectPool	= NULL;
BOOL CreateDataPool()
{
	g_pRoleDataObjectPool		= new SharedMemory<RoleDataObject>("Role", 1024);
	g_pGlobalDataObjectPool		= new SharedMemory<GlobalDataObject>("Global", 1024);
	g_pBagDataObjectPool		= new SharedMemory<BagDataObject>("Bag", 1024);
	g_pCopyDataObjectPool		= new SharedMemory<CopyDataObject>("Copy", 1024);
	g_pEquipDataObjectPool		= new SharedMemory<EquipDataObject>("Equip", 1024);
	g_pPetDataObjectPool		= new SharedMemory<PetDataObject>("Pet", 1024);
	g_pPartnerDataObjectPool	= new SharedMemory<PartnerDataObject>("Partner", 1024);
	g_pGuildDataObjectPool		= new SharedMemory<GuildDataObject>("Guild", 1024);
	g_pMemberDataObjectPool		= new SharedMemory<MemberDataObject>("GuildMember", 1024);
	g_pTaskDataObjectPool		= new SharedMemory<TaskDataObject>("Task", 1024);
	g_pMountDataObjectPool		= new SharedMemory<MountDataObject>("Mount", 1024);
	g_pMailDataObjectPool		= new SharedMemory<MailDataObject>("Mail", 1024);
	g_pGroupMailDataObjectPool  = new SharedMemory<GroupMailDataObject>("GroupMail", 1024);
	g_pActivityDataObjectPool	= new SharedMemory<ActivityDataObject>("Activity", 1024);
	g_pCounterDataObjectPool    = new SharedMemory<CounterDataObject>("Counter", 1024);
	g_pFriendDataObjectPool    = new SharedMemory<FriendDataObject>("Friend", 1024);


	g_pRoleDataObjectPool->InitToMap();
	g_pGlobalDataObjectPool->InitToMap();
	g_pBagDataObjectPool->InitToMap();
	g_pCopyDataObjectPool->InitToMap();
	g_pEquipDataObjectPool->InitToMap();
	g_pPetDataObjectPool->InitToMap();
	g_pPartnerDataObjectPool->InitToMap();
	g_pGuildDataObjectPool->InitToMap();
	g_pMemberDataObjectPool->InitToMap();
	g_pTaskDataObjectPool->InitToMap();
	g_pMountDataObjectPool->InitToMap();
	g_pMailDataObjectPool->InitToMap();
	g_pGroupMailDataObjectPool->InitToMap();
	g_pActivityDataObjectPool->InitToMap();
	g_pCounterDataObjectPool->InitToMap();
	g_pFriendDataObjectPool->InitToMap();
	return TRUE;
}

std::string GenDataName(std::string strName)
{
	return strName + "_" + CommonConvert::IntToString(CGameService::GetInstancePtr()->GetServerID());
}
