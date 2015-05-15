#include "stdafx.h"
#include "SloongThreadPool.h"
using namespace SoaringLoong::Universal;

list<ThreadParam*>* SoaringLoong::Universal::CThreadPool::m_pJobList;

HANDLE SoaringLoong::Universal::CThreadPool::g_pMutex;
CRITICAL_SECTION g_pData;
SoaringLoong::Universal::CThreadPool::CThreadPool()
{
	m_pThreadList = new vector<HANDLE>;
	m_pJobList = new list<ThreadParam*>;
}

SoaringLoong::Universal::CThreadPool::~CThreadPool()
{
	SAFE_DELETE(m_pThreadList);
	SAFE_DELETE(m_pJobList);
}

void SoaringLoong::Universal::CThreadPool::Initialize(int nThreadNum)
{
	for (int i = 0; i < nThreadNum;i++)
	{
		auto pThread = CreateThread(NULL, NULL, ThreadWorkLoop, NULL, NULL, NULL);
		m_pThreadList->push_back(pThread);
	}
}

void SoaringLoong::Universal::CThreadPool::Start()
{
	g_pMutex = CreateMutex(NULL, FALSE, _T("SloongThreadPoolMutex"));
// 	for each (auto& item in *m_pThreadList)
// 	{
// 	}
}

DWORD SoaringLoong::Universal::CThreadPool::ThreadWorkLoop(LPVOID lpParam)
{
	while (true)
	{
		if (m_pJobList->empty() || 0 == m_pJobList->size())
		{
			Sleep(1);
			continue;
		}
		WaitForSingleObject(g_pMutex, INFINITE);
		if (m_pJobList->empty() || 0 == m_pJobList->size())
		{
			ReleaseMutex(g_pMutex);
			continue;
		}
		auto pItem = m_pJobList->front();
		m_pJobList->pop_front();
		ReleaseMutex(g_pMutex);
		(*pItem->pJob)(pItem->pParam);
		SAFE_DELETE(pItem);
	}
	return 0;
}

void SoaringLoong::Universal::CThreadPool::End()
{

}

int SoaringLoong::Universal::CThreadPool::AddTask( LPCALLBACKFUNC pJob, LPVOID pParam )
{
	ThreadParam* pItem = new ThreadParam();
	pItem->pJob = pJob;
	pItem->pParam = pParam;
	WaitForSingleObject(g_pMutex, INFINITE);
	m_pJobList->push_back(pItem);
	ReleaseMutex(g_pMutex);
	return m_pJobList->size();
}

void SoaringLoong::Universal::CThreadPool::RemoveTask(int index)
{
	
}

int SoaringLoong::Universal::CThreadPool::GetTaskTotal()
{
	return m_pJobList->size();
}

HANDLE SoaringLoong::Universal::CThreadPool::GetJobListMutex()
{
	return g_pMutex;
}
