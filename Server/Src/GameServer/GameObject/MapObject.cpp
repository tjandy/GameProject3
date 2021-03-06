﻿#include "stdafx.h"
#include "MapObject.h"
#include "CommandDef.h"
#include "Log.h"
#include "CommonFunc.h"
#include "../Scene.h"


CMapObject::CMapObject()
{
	m_pScene = NULL;

	m_pObjectNext = NULL;

	m_pObjectPrev = NULL;
}

CMapObject::~CMapObject()
{
	m_pScene = NULL;

	m_pObjectNext = NULL;

	m_pObjectPrev = NULL;
}

BOOL CMapObject::SetUpdate()
{
	//m_pScene->AddToUpdateList(this);

	return TRUE;
}


CScene* CMapObject::GetScene()
{
	return m_pScene;
}

VOID CMapObject::SetScene( CScene* pScene )
{
	m_pScene = pScene;

	return ;
}
