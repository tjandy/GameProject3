﻿#include "stdafx.h"
#include "Connection.h"
#include "CommandDef.h"
#include <boost/asio/placeholders.hpp>
#include "PacketHeader.h"
#include "../ServerEngine/Log.h"
#include "../ServerEngine/DataBuffer.h"
#include "../ServerEngine/CommonFunc.h"



CConnection::CConnection(boost::asio::io_service& ioservice): m_hSocket(ioservice)
{
	m_pDataHandler		= NULL;

	m_dwDataLen			= 0;

	m_bConnected		= FALSE;

	m_u64ConnData        = 0;

	m_dwConnID          = 0;

	m_pCurRecvBuffer    = NULL;

	m_pBufPos           = m_pRecvBuf;

	m_nCheckNo          = 0;

	m_IsSending			= FALSE;
}

CConnection::~CConnection(void)
{
	m_pDataHandler		= NULL;

	m_dwDataLen			= 0;

	m_u64ConnData        = 0;

	m_dwConnID          = 0;

	m_bConnected		= FALSE;

	m_pCurRecvBuffer    = NULL;

	m_pBufPos           = m_pRecvBuf;

	m_nCheckNo          = 0;

	m_IsSending			= FALSE;
}

BOOL CConnection::DoReceive()
{
	//boost::asio::async_read(m_hSocket, boost::asio::buffer(m_pRecvBuf + m_dwDataLen,CONST_BUFF_SIZE - m_dwDataLen), boost::bind(&CConnection::handReaddata, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));


	//boost::asio::async_read_some(m_hSocket, boost::asio::buffer(m_pRecvBuf + m_dwDataLen,CONST_BUFF_SIZE - m_dwDataLen), boost::bind(&CConnection::handReaddata, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

	m_hSocket.async_read_some(boost::asio::buffer(m_pRecvBuf + m_dwDataLen, RECV_BUF_SIZE - m_dwDataLen), boost::bind(&CConnection::HandReaddata, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

	return TRUE;
}

UINT32 CConnection::GetConnectionID()
{
	return m_dwConnID;
}

UINT64 CConnection::GetConnectionData()
{
	return m_u64ConnData;
}

void CConnection::SetConnectionID( UINT32 dwConnID )
{
	ASSERT(dwConnID != 0);
	ASSERT(!m_bConnected);

	m_dwConnID = dwConnID;

	return ;
}

VOID CConnection::SetConnectionData( UINT64 dwData )
{
	ASSERT(m_dwConnID != 0);
	m_u64ConnData = dwData;

	return ;
}


BOOL CConnection::ExtractBuffer()
{
	//在这方法里返回FALSE。
	//会在外面导致这个连接被关闭。

	ERROR_RETURN_TRUE(m_dwDataLen > 0);

	while(TRUE)
	{
		if(m_pCurRecvBuffer != NULL)
		{
			if ((m_pCurRecvBuffer->GetTotalLenth() + m_dwDataLen ) < m_pCurBufferSize)
			{
				memcpy(m_pCurRecvBuffer->GetBuffer() + m_pCurRecvBuffer->GetTotalLenth(), m_pBufPos, m_dwDataLen);
				m_dwDataLen = 0;
				m_pBufPos = m_pRecvBuf;
				m_pCurRecvBuffer->SetTotalLenth(m_pCurRecvBuffer->GetTotalLenth() + m_dwDataLen);
				break;
			}
			else
			{
				memcpy(m_pCurRecvBuffer->GetBuffer() + m_pCurRecvBuffer->GetTotalLenth(), m_pBufPos, m_pCurBufferSize - m_pCurRecvBuffer->GetTotalLenth());
				m_dwDataLen -= m_pCurBufferSize - m_pCurRecvBuffer->GetTotalLenth();
				m_pBufPos += m_pCurBufferSize - m_pCurRecvBuffer->GetTotalLenth();
				m_pCurRecvBuffer->SetTotalLenth(m_pCurBufferSize);
				m_pDataHandler->OnDataHandle(m_pCurRecvBuffer, this);
				m_pCurRecvBuffer = NULL;
			}
		}

		if(m_dwDataLen < sizeof(PacketHeader))
		{
			break;
		}

		PacketHeader* pHeader = (PacketHeader*)m_pBufPos;
		//////////////////////////////////////////////////////////////////////////
		//在这里对包头进行检查, 如果不合法就要返回FALSE;
		/*
		1.首先验证包的验证吗
		2.包的长度
		3.包的序号*/
		if(pHeader->CheckCode != 0x88)
		{
			return FALSE;
		}

		if(pHeader->dwSize > 1024 * 1024)
		{
			return FALSE;
		}

		if(pHeader->dwMsgID > 4999999)
		{
			return FALSE;
		}
		/*if(m_nCheckNo == 0)
		{
			m_nCheckNo = pHeader->dwPacketNo - pHeader->wCommandID^pHeader->dwSize;
		}
		else
		{
			if(pHeader->dwPacketNo = pHeader->wCommandID^pHeader->dwSize+m_nCheckNo)
			{
				m_nCheckNo += 1;
			}
			else
			{
				return FALSE;
			}*/



		ERROR_RETURN_FALSE(pHeader->dwSize != 0);

		UINT32 dwPacketSize = pHeader->dwSize;

		//////////////////////////////////////////////////////////////////////////
		if((dwPacketSize > m_dwDataLen)  && (dwPacketSize < RECV_BUF_SIZE))
		{
			break;
		}

		if (dwPacketSize <= m_dwDataLen)
		{
			IDataBuffer* pDataBuffer =  CBufferManagerAll::GetInstancePtr()->AllocDataBuff(dwPacketSize);

			memcpy(pDataBuffer->GetBuffer(), m_pBufPos, dwPacketSize);

			m_dwDataLen -= dwPacketSize;

			m_pBufPos += dwPacketSize;

			pDataBuffer->SetTotalLenth(dwPacketSize);

			m_pDataHandler->OnDataHandle(pDataBuffer, this);
		}
		else
		{
			IDataBuffer* pDataBuffer =  CBufferManagerAll::GetInstancePtr()->AllocDataBuff(dwPacketSize);
			memcpy(pDataBuffer->GetBuffer(), m_pBufPos, m_dwDataLen);

			pDataBuffer->SetTotalLenth(m_dwDataLen);
			m_dwDataLen = 0;
			m_pBufPos = m_pRecvBuf;
			m_pCurRecvBuffer = pDataBuffer;
			m_pCurBufferSize = dwPacketSize;
		}
	}

	if(m_dwDataLen > 0)
	{
		memmove(m_pRecvBuf, m_pBufPos, m_dwDataLen);
	}

	m_pBufPos = m_pRecvBuf;

	return TRUE;
}

BOOL CConnection::Close()
{

	m_pDataHandler->OnCloseConnect(this);
	m_hSocket.close();
	m_bConnected        = FALSE;
	m_dwDataLen         = 0;
	m_IsSending			= FALSE;
	return TRUE;
}

BOOL CConnection::HandleRecvEvent(UINT32 dwBytes)
{
	m_dwDataLen += dwBytes;

	if(!ExtractBuffer())
	{
		return FALSE;
	}

	if (!DoReceive())
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CConnection::SetDataHandler( IDataHandler* pHandler )
{
	ERROR_RETURN_FALSE(pHandler != NULL);

	m_pDataHandler = pHandler;

	return TRUE;
}

boost::asio::ip::tcp::socket& CConnection::GetSocket()
{
	return m_hSocket;
}

BOOL CConnection::IsConnectionOK()
{
	//m_hSocket.is_open()

	return m_bConnected;
}

BOOL CConnection::SetConnectionOK( BOOL bOk )
{
	m_bConnected = bOk;

	m_LastRecvTick = CommonFunc::GetTickCount();

	return TRUE;
}


BOOL CConnection::Clear()
{
	m_bConnected = FALSE;

	m_u64ConnData = 0;

	m_dwDataLen = 0;

	m_dwIpAddr  = 0;

	m_pBufPos   = m_pRecvBuf;

	if(m_pCurRecvBuffer != NULL)
	{
		m_pCurRecvBuffer->Release();
	}

	m_pCurRecvBuffer = NULL;

	m_nCheckNo = 0;

	m_IsSending	= FALSE;

	IDataBuffer* pBuff = NULL;
	while(m_SendBuffList.pop(pBuff))
	{
		pBuff->Release();
	}

	return TRUE;
}


BOOL CConnection::SendBuffer(IDataBuffer* pBuff)
{
	m_SendBuffList.push(pBuff);
	DoSend();
	return TRUE;
}

BOOL CConnection::SendMessage(UINT32 dwMsgID, UINT64 u64TargetID, UINT32 dwUserData, const char* pData, UINT32 dwLen)
{
	IDataBuffer* pDataBuffer = CBufferManagerAll::GetInstancePtr()->AllocDataBuff(dwLen + sizeof(PacketHeader));
	ERROR_RETURN_FALSE(pDataBuffer != NULL);

	PacketHeader* pHeader = (PacketHeader*)pDataBuffer->GetBuffer();
	pHeader->CheckCode = 0x88;
	pHeader->dwUserData = dwUserData;
	pHeader->u64TargetID = u64TargetID;
	pHeader->dwSize = dwLen + sizeof(PacketHeader);
	pHeader->dwMsgID = dwMsgID;
	pHeader->dwPacketNo = 1;

	memcpy(pDataBuffer->GetBuffer() + sizeof(PacketHeader), pData, dwLen);

	pDataBuffer->SetTotalLenth(pHeader->dwSize);

	return SendBuffer(pDataBuffer);
}

BOOL CConnection::DoSend()
{
	///如果正在发送中就直接返回
	if (m_IsSending)
	{
		return FALSE;
	}

	m_IsSending = TRUE;
	IDataBuffer* pFirstBuff = NULL;
	IDataBuffer* pBuffer = NULL;
	IDataBuffer* pSendingBuffer = NULL;
	int nSendSize = 0;
	int nCurPos = 0;
	while(m_SendBuffList.pop(pBuffer))
	{
		nSendSize += pBuffer->GetTotalLenth();

		if(pFirstBuff == NULL)
		{
			if(nSendSize >= RECV_BUF_SIZE)
			{
				pSendingBuffer = pBuffer;
				break;
			}

			pFirstBuff = pBuffer;
		}
		else
		{
			if(pSendingBuffer == NULL)
			{
				pSendingBuffer = CBufferManagerAll::GetInstancePtr()->AllocDataBuff(RECV_BUF_SIZE);
				pFirstBuff->CopyTo(pSendingBuffer->GetBuffer() + nCurPos, pFirstBuff->GetTotalLenth());
				pSendingBuffer->SetTotalLenth(pSendingBuffer->GetTotalLenth() + pFirstBuff->GetTotalLenth());
				nCurPos += pFirstBuff->GetTotalLenth();
				pFirstBuff->Release();
				pFirstBuff = NULL;
			}

			pBuffer->CopyTo(pSendingBuffer->GetBuffer() + nCurPos, pBuffer->GetTotalLenth());
			pSendingBuffer->SetTotalLenth(pSendingBuffer->GetTotalLenth() + pBuffer->GetTotalLenth());
			nCurPos += pBuffer->GetTotalLenth();
			pBuffer->Release();

			if(nSendSize >= RECV_BUF_SIZE)
			{
				break;
			}
		}
	}

	if(pSendingBuffer == NULL)
	{
		pSendingBuffer = pFirstBuff;
	}

	if(pSendingBuffer == NULL)
	{
		m_IsSending = FALSE;
		return FALSE;
	}

	return TRUE;
}


void CConnection::HandReaddata(const boost::system::error_code& error, size_t len)
{
	HandleRecvEvent(len);
}


CConnectionMgr::CConnectionMgr()
{
	m_pFreeConnRoot = NULL;
	m_pFreeConnTail = NULL;
}

CConnectionMgr::~CConnectionMgr()
{
	DestroyAllConnection();
	m_pFreeConnRoot = NULL;
	m_pFreeConnTail = NULL;
}

CConnection* CConnectionMgr::CreateConnection()
{
	ERROR_RETURN_NULL(m_pFreeConnRoot != NULL);

	CConnection* pTemp = NULL;
	m_CritSecConnList.Lock();
	if(m_pFreeConnRoot == m_pFreeConnTail)
	{
		pTemp = m_pFreeConnRoot;
		m_pFreeConnTail = m_pFreeConnRoot = NULL;
	}
	else
	{
		pTemp = m_pFreeConnRoot;
		m_pFreeConnRoot = pTemp->m_pNext;
		pTemp->m_pNext = NULL;
	}
	m_CritSecConnList.Unlock();
	ERROR_RETURN_NULL(pTemp->GetConnectionID() != 0);
	ERROR_RETURN_NULL(pTemp->IsConnectionOK() == FALSE);
	return pTemp;
}

CConnection* CConnectionMgr::GetConnectionByConnID( UINT32 dwConnID )
{
	UINT32 dwIndex = dwConnID % m_vtConnList.size();

	ERROR_RETURN_NULL(dwIndex < m_vtConnList.size())

	CConnection* pConnect = m_vtConnList.at(dwIndex - 1);
	if(pConnect->GetConnectionID() != dwConnID)
	{
		return NULL;
	}

	return pConnect;
}


CConnectionMgr* CConnectionMgr::GetInstancePtr()
{
	static CConnectionMgr ConnectionMgr;

	return &ConnectionMgr;
}


BOOL CConnectionMgr::DeleteConnection(CConnection* pConnection)
{
	ERROR_RETURN_FALSE(pConnection != NULL);

	m_CritSecConnList.Lock();

	if(m_pFreeConnTail == NULL)
	{
		if(m_pFreeConnRoot != NULL)
		{
			ASSERT_FAIELD;
		}

		m_pFreeConnTail = m_pFreeConnRoot = pConnection;
	}
	else
	{
		m_pFreeConnTail->m_pNext = pConnection;

		m_pFreeConnTail = pConnection;

		m_pFreeConnTail->m_pNext = NULL;

	}

	m_CritSecConnList.Unlock();

	UINT32 dwConnID = pConnection->GetConnectionID();

	pConnection->Clear();

	dwConnID += m_vtConnList.size();

	pConnection->SetConnectionID(dwConnID);

	return TRUE;
}

BOOL CConnectionMgr::CloseAllConnection()
{
	CConnection* pConn = NULL;
	for(size_t i = 0; i < m_vtConnList.size(); i++)
	{
		pConn = m_vtConnList.at(i);
		pConn->Close();
	}

	return TRUE;
}

BOOL CConnectionMgr::DestroyAllConnection()
{
	CConnection* pConn = NULL;
	for(size_t i = 0; i < m_vtConnList.size(); i++)
	{
		pConn = m_vtConnList.at(i);
		pConn->Close();
		delete pConn;
	}

	m_vtConnList.clear();

	return TRUE;
}

BOOL CConnectionMgr::CheckConntionAvalible()
{
	return TRUE;
	UINT32 curTick = CommonFunc::GetTickCount();

	for(std::vector<CConnection*>::size_type i = 0; i < m_vtConnList.size(); i++)
	{
		CConnection* pTemp = m_vtConnList.at(i);
		if(!pTemp->IsConnectionOK())
		{
			continue;
		}

		if(curTick > (pTemp->m_LastRecvTick + 30000))
		{
			pTemp->Close();
		}
	}

	return TRUE;
}
BOOL CConnectionMgr::InitConnectionList(UINT32 nMaxCons, boost::asio::io_service& ioservice)
{
	ERROR_RETURN_FALSE(m_pFreeConnRoot == NULL);
	ERROR_RETURN_FALSE(m_pFreeConnTail == NULL);

	m_vtConnList.assign(nMaxCons, NULL);
	for(UINT32 i = 0; i < nMaxCons; i++)
	{
		CConnection* pConn = new CConnection(ioservice);

		m_vtConnList[i] = pConn;

		pConn->SetConnectionID(i + 1) ;

		if (m_pFreeConnRoot == NULL)
		{
			m_pFreeConnRoot = pConn;
			pConn->m_pNext = NULL;
			m_pFreeConnTail = pConn;
		}
		else
		{
			m_pFreeConnTail->m_pNext = pConn;
			m_pFreeConnTail = pConn;
			m_pFreeConnTail->m_pNext = NULL;
		}
	}

	return TRUE;
}


