﻿#ifndef __COPY_DATA_OBJECT_H__
#define __COPY_DATA_OBJECT_H__
#include "DBInterface/DBInterface.h"
struct CopyDataObject : public ShareObject
{
	CopyDataObject()
	{
		m_uRoleID = 0;
		m_dwChaper = 0;
		m_dwBattleTimes = 0;
		m_dwResetTimes = 0;
		m_dwStar = 0;
		m_dwCopyID = 0;
	}

	UINT64 m_uRoleID;
	UINT32 m_dwCopyID;
	UINT32 m_dwChaper;
	UINT32 m_dwBattleTimes;
	UINT32 m_dwResetTimes;
	UINT32 m_dwStar;

	BOOL Create(IDBInterface* pDB)
	{
		return TRUE;
	}

	BOOL Update(IDBInterface* pDB)
	{
		return TRUE;
	}

	BOOL Delete(IDBInterface* pDB)
	{
		return TRUE;
	}
};


#endif //__COPY_DATA_OBJECT_H__