﻿#ifndef __EQUIP_DATA_OBJECT_H__
#define __EQUIP_DATA_OBJECT_H__

#include "SharedMemory.h"
#include "DBInterface/DBStoredProc.h"
#include "DBInterface/DBInterface.h"
struct EquipDataObject : public ShareObject
{
	EquipDataObject()
	{
		m_uRoleID = 0;
		m_uGuid = 0;
		m_EquipID = 0;
		m_StrengthLvl = 0;
		m_RefineLevel = 0;
		m_StarLevel = 0;
		m_RefineExp = 0;
		m_StarExp = 0;
		m_IsUsing = 0;
	}

	UINT64 m_uRoleID;		//角色ID
	UINT64 m_uGuid;			//guid
	UINT32 m_EquipID;		//装备ID
	INT32  m_StrengthLvl;   //强化等级
	INT32  m_RefineLevel;   //精炼等级
	INT32  m_StarLevel;		//星级
	INT32  m_RefineExp;		//精验的经验
	INT32  m_StarExp;		//星级经验
	BOOL   m_IsUsing;       //当前使用中

	BOOL Create(IDBInterface* pDB)
	{
		static CDBStoredProcedure csp("REPLACE INTO equip (id, title, content, time, type, channel, item, itemcount) \
			VALUES(?,?,?,?,?,?,?,?,?);");

		csp.set_uint64(0, m_uGuid);
		csp.set_uint64(1, m_uRoleID);
		csp.set_uint32(2, m_EquipID);
		csp.set_int32(3, m_StrengthLvl);
		csp.set_int32(4, m_RefineLevel);
		csp.set_int32(5, m_StarLevel);
		csp.set_int32(6, m_RefineExp);
		csp.set_int32(7, m_StarExp);
		csp.set_bool(8, m_IsUsing);
		pDB->Execute(&csp);
		return TRUE;
	}

	BOOL Update(IDBInterface* pDB)
	{
		static CDBStoredProcedure csp("REPLACE INTO equip (id, title, content, time, type, channel, item, itemcount) \
			VALUES(?,?,?,?,?,?,?,?,?);");
		csp.set_uint64(0, m_uGuid);
		csp.set_uint64(1, m_uRoleID);
		csp.set_uint32(2, m_EquipID);
		csp.set_int32(3, m_StrengthLvl);
		csp.set_int32(4, m_RefineLevel);
		csp.set_int32(5, m_StarLevel);
		csp.set_int32(6, m_RefineExp);
		csp.set_int32(7, m_StarExp);
		csp.set_bool(8, m_IsUsing);
		pDB->Execute(&csp);
		return TRUE;
	}

	BOOL Delete(IDBInterface* pDB)
	{


		return TRUE;
	}
};



#endif //__EQUIP_DATA_OBJECT_H__