#include "StdAfx.h"
#include "TableFrameSink.h"
#include "FvMask.h"

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#define IDI_TIMER_XIAO_HU			1 //小胡

#define TIME_XIAO_HU  8

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_cbNiaoCount = 0;
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory( m_GangScore,sizeof(m_GangScore) );
	ZeroMemory( m_lGameScore,sizeof(m_lGameScore) );
	ZeroMemory( m_wLostFanShu,sizeof(m_wLostFanShu) );

	//出牌信息
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//发牌信息
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	ZeroMemory(m_cbRepertoryCard_HZ,sizeof(m_cbRepertoryCard_HZ));
	
	//运行变量
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;

	//状态变量
	m_bSendStatus=false;
	m_bGangStatus = false;
	m_bGangOutStatus = false;

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//结束信息
	m_cbChiHuCard=0;
	ZeroMemory( m_dwChiHuKind,sizeof(m_dwChiHuKind) );
	memset( m_wProvider,INVALID_CHAIR,sizeof(m_wProvider) );

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	m_cbGameTypeIdex = 0;
	m_dwGameRuleIdex = 0;

	ZeroMemory( m_lStartHuScore,sizeof(m_lStartHuScore) );
	ZeroMemory( m_cbCardDataNiao,sizeof(m_cbCardDataNiao) );
	
	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询
void * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL)
		return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	//游戏变量
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory( m_GangScore,sizeof(m_GangScore) );
	ZeroMemory( m_lGameScore,sizeof(m_lGameScore) );
	ZeroMemory( m_wLostFanShu,sizeof(m_wLostFanShu) );

	//出牌信息
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//发牌信息
	m_cbNiaoCount = 0;
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	ZeroMemory(m_cbRepertoryCard_HZ,sizeof(m_cbRepertoryCard_HZ));

	//运行变量
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;
	m_wBankerUser = INVALID_CHAIR;

	//状态变量
	m_bSendStatus=false;
	m_bGangStatus = false;
	m_bGangOutStatus = false;

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//结束信息
	m_cbChiHuCard=0;
	ZeroMemory(m_dwChiHuKind,sizeof(m_dwChiHuKind));
	
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		m_ChiHuRight[i].SetEmpty();
		m_StartHuRight[i].SetEmpty();
	}

	ZeroMemory( m_lStartHuScore,sizeof(m_lStartHuScore) );
	ZeroMemory( m_cbCardDataNiao,sizeof(m_cbCardDataNiao) );

	return;
}


//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	if (hasRule(GAME_TYPE_ZZ_HONGZHONG))
	{
		Shuffle(m_cbRepertoryCard_HZ,MAX_REPERTORY_HZ);
	}
	else
	{
		Shuffle(m_cbRepertoryCard,MAX_REPERTORY);
	}

	if (m_cbGameTypeIdex == GAME_TYPE_ZZ)
	{
		GameStart_ZZ();
	}
	else if (m_cbGameTypeIdex == GAME_TYPE_CS)
	{
		GameStart_CS();
	}
	//test
// 	BYTE byTest[] = {
// 		0x01,0x02,0x03,0x04,0x05,0x06,0x07,
// 		0x01,0x02,0x03,0x04,0x05,0x06,0x07,
// 		0x01,0x02,0x03,0x04,0x05,0x06,0x07,
// 
// 		0x21,0x22,0x14,0x15,0x16,0x17,
// 		0x08,0x12,0x13,0x14,0x15,0x16,0x17,
// 		0x19,0x12,0x13,0x14,0x15,0x16,0x17,
// 
// 		0x21,0x22,0x23,0x24,0x25,0x26,0x27,
// 		0x21,0x22,0x23,0x11,0x24,0x25,0x26,0x27,
// 
// 		0x12,0x13,0x23,0x24,0x25,0x26,0x27,0x28,0x18,0x18,0x18,0x08,0x08,
// 
// 		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x29,0x29,0x29,0x28,
// 
// 		0x11,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x19,0x19,0x28,
// 
// 		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x09,0x09,0x09,0x11,
// 	};
//	CopyMemory( m_cbRepertoryCard,byTest,sizeof(byTest) );
//	m_wBankerUser = 0;
	//end test


	return true;
}

void CTableFrameSink::Shuffle(BYTE* pRepertoryCard,int nCardCount)
{
	m_cbLeftCardCount = nCardCount;
	m_GameLogic.RandCardData(pRepertoryCard,nCardCount);

	//分发扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if( m_pITableFrame->GetTableUserItem(i) != NULL )
		{
			m_cbLeftCardCount-=(MAX_COUNT-1);
			m_GameLogic.SwitchToCardIndex(&pRepertoryCard[m_cbLeftCardCount],MAX_COUNT-1,m_cbCardIndex[i]);
		}
	}
}

void CTableFrameSink::GameStart_ZZ()
{
	//混乱扑克
	LONG lSiceCount = MAKELONG(MAKEWORD(rand()%6+1,rand()%6+1),MAKEWORD(rand()%6+1,rand()%6+1));
	m_wBankerUser = ((BYTE)(lSiceCount>>24)+(BYTE)(lSiceCount>>16)-1)%GAME_PLAYER;

	//设置变量
	m_cbProvideCard=0;
	m_wProvideUser=INVALID_CHAIR;
	m_wCurrentUser=m_wBankerUser;

	//构造数据
	CMD_S_GameStart GameStart;
	GameStart.lSiceCount=lSiceCount;
	GameStart.wBankerUser=m_wBankerUser;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.cbLeftCardCount = m_cbLeftCardCount;

	m_pITableFrame->SetGameStatus(GS_MJ_PLAY);
	GameStart.cbXiaoHuTag = 0;

	//发送数据
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置变量
		GameStart.cbUserAction = WIK_NULL;//m_cbUserAction[i];

		ZeroMemory(GameStart.cbCardData,sizeof(GameStart.cbCardData));
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameStart.cbCardData);

		if( m_pITableFrame->GetTableUserItem(i)->IsAndroidUser() )
		{
			BYTE bIndex = 1;
			for( WORD j=0; j<GAME_PLAYER; j++ )
			{
				if( j == i ) continue;
				m_GameLogic.SwitchToCardData(m_cbCardIndex[j],&GameStart.cbCardData[MAX_COUNT*bIndex++]);
			}
		}

		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}
	m_bSendStatus = true;
    DispatchCardData(m_wCurrentUser);
}
void CTableFrameSink::GameStart_CS()
{
	//混乱扑克
	LONG lSiceCount = MAKELONG(MAKEWORD(rand()%6+1,rand()%6+1),MAKEWORD(rand()%6+1,rand()%6+1));
	m_wBankerUser = ((BYTE)(lSiceCount>>24)+(BYTE)(lSiceCount>>16)-1)%GAME_PLAYER;

	//设置变量
	m_cbProvideCard=0;
	m_wProvideUser=INVALID_CHAIR;
	m_wCurrentUser=m_wBankerUser;

	//构造数据
	CMD_S_GameStart GameStart;
	GameStart.lSiceCount=lSiceCount;
	GameStart.wBankerUser=m_wBankerUser;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.cbLeftCardCount = m_cbLeftCardCount;

	bool bInXaoHu = false;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//胡牌判断
		CChiHuRight chr;
		m_cbUserAction[i] |= AnalyseChiHuCardCS_XIAOHU(m_cbCardIndex[i],chr);
		if (m_cbUserAction[i] != WIK_NULL)
		{
			bInXaoHu = true;
		}
	}

	if (bInXaoHu)
	{
		m_pITableFrame->SetGameStatus(GS_MJ_XIAOHU);
		m_pITableFrame->SetGameTimer(IDI_TIMER_XIAO_HU,(TIME_XIAO_HU)*1000,1,0);
		GameStart.cbXiaoHuTag =1;
	}
	else
	{
		m_pITableFrame->SetGameStatus(GS_MJ_PLAY);
		GameStart.cbXiaoHuTag = 0;
	}

	//发送数据
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置变量
		GameStart.cbUserAction = WIK_NULL;//m_cbUserAction[i];

		ZeroMemory(GameStart.cbCardData,sizeof(GameStart.cbCardData));
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameStart.cbCardData);

		if( m_pITableFrame->GetTableUserItem(i)->IsAndroidUser() )
		{
			BYTE bIndex = 1;
			for( WORD j=0; j<GAME_PLAYER; j++ )
			{
				if( j == i ) continue;
				m_GameLogic.SwitchToCardData(m_cbCardIndex[j],&GameStart.cbCardData[MAX_COUNT*bIndex++]);
			}
		}

		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	if (GameStart.cbXiaoHuTag == 1)
	{
		SendOperateNotify();
	}
	else
	{
		m_bSendStatus = true;
		DispatchCardData(m_wCurrentUser);
	}
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			GameEnd.wLeftUser = INVALID_CHAIR;

			//设置中鸟数据
			ZeroMemory(GameEnd.cbCardDataNiao,sizeof(GameEnd.cbCardDataNiao));
			GameEnd.cbNiaoPick = m_cbNiaoPick;
			GameEnd.cbNiaoCount = m_cbNiaoCount;
			for (int i = 0;i<MAX_NIAO_CARD && i< m_cbNiaoCount;i++)
			{
				GameEnd.cbCardDataNiao[i] = m_cbCardDataNiao[i];
			}
			
			//结束信息
			WORD wWinner = INVALID_CHAIR;
			BYTE cbLeftUserCount = 0;			//判断是否流局
			bool bUserStatus[GAME_PLAYER];		//
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
				m_ChiHuRight[i].GetRightData( &GameEnd.dwChiHuRight[i],MAX_RIGHT_COUNT );
				m_StartHuRight[i].GetRightData( &GameEnd.dwStartHuRight[i],MAX_RIGHT_COUNT );
				
				//流局玩家数
				if( m_ChiHuRight[i].IsEmpty() ) cbLeftUserCount++;
				//当前玩家状态
				if( NULL != m_pITableFrame->GetTableUserItem(i) ) bUserStatus[i] = true;
				else bUserStatus[i] = false;
			}
			
			LONGLONG lGangScore[GAME_PLAYER];
			ZeroMemory(&lGangScore,sizeof(lGangScore));
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				for( BYTE j = 0; j < m_GangScore[i].cbGangCount; j++ )
				{
					for( WORD k = 0; k < GAME_PLAYER; k++ )
						lGangScore[k] += m_GangScore[i].lScore[j][k];
				}
			}

			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				m_lGameScore[i] += lGangScore[i];
				m_lGameScore[i] += m_lStartHuScore[i];

				//防止负分
				IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
				if( GAME_GENRE_SCORE!=m_pGameServiceOption->wServerType &&
					m_lGameScore[i] < 0L && -m_lGameScore[i] > pUserItem->GetUserScore() )
					m_lGameScore[i] = -pUserItem->GetUserScore();
			}

			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

			int	lGameTaxs[GAME_PLAYER];				//
			ZeroMemory(&lGameTaxs,sizeof(lGameTaxs));
			//统计积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if( NULL == m_pITableFrame->GetTableUserItem(i) ) continue;

				//设置积分
				if (m_lGameScore[i]>0L)
				{
					lGameTaxs[i] = m_pITableFrame->CalculateRevenue(i,m_lGameScore[i]);
					m_lGameScore[i] -= lGameTaxs[i];
				}
				
				BYTE ScoreKind;
				if( m_lGameScore[i] > 0L ) ScoreKind = SCORE_TYPE_WIN;
				else if( m_lGameScore[i] < 0L ) ScoreKind = SCORE_TYPE_LOSE;
				else ScoreKind = SCORE_TYPE_DRAW;

				ScoreInfoArray[i].lScore   = m_lGameScore[i];
				ScoreInfoArray[i].lRevenue = lGameTaxs[i];
				ScoreInfoArray[i].cbType   = ScoreKind;

			}

			//写入积分
			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			CopyMemory( GameEnd.lGameScore,m_lGameScore,sizeof(GameEnd.lGameScore) );
			CopyMemory( GameEnd.lGangScore,lGangScore,sizeof(GameEnd.lGangScore) );
			CopyMemory( GameEnd.wProvideUser,m_wProvider,sizeof(GameEnd.wProvideUser) );
			CopyMemory( GameEnd.lGameTax,lGameTaxs,sizeof(GameEnd.lGameTax));
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				CopyMemory( GameEnd.wLostFanShu[i],m_wLostFanShu[i],sizeof(m_wLostFanShu[i]) );
				CopyMemory(GameEnd.WeaveItemArray[i],m_WeaveItemArray[i],sizeof(m_WeaveItemArray[i]));
			}
			CopyMemory( GameEnd.lStartHuScore,m_lStartHuScore,sizeof(GameEnd.lStartHuScore) );

			

			//组合扑克
			CopyMemory(GameEnd.cbWeaveCount,m_cbWeaveItemCount,sizeof(GameEnd.cbWeaveCount));

			//发送结束信息
			m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd) );
			m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd) );

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_MJ_FREE);

			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			GameEnd.wLeftUser = INVALID_CHAIR;

			//设置变量
			memset( GameEnd.wProvideUser,INVALID_CHAIR,sizeof(GameEnd.wProvideUser) );

			//拷贝扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_MJ_FREE);

			return true;
		}
	case GER_NETWORK_ERROR:		//网络错误
	case GER_USER_LEAVE:		//用户强退
		{
			m_bTrustee[wChairID] = true;

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);
	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_MJ_FREE:	//空闲状态
		{
			//变量定义
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));

			//构造数据
			StatusFree.wBankerUser=m_wBankerUser;
			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusFree.bTrustee,m_bTrustee,sizeof(m_bTrustee));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_MJ_PLAY:	//游戏状态
		{
			//变量定义
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusPlay.bTrustee,m_bTrustee,sizeof(m_bTrustee));

			//状态变量
			StatusPlay.cbActionCard=m_cbProvideCard;
			StatusPlay.cbLeftCardCount=m_cbLeftCardCount;
			StatusPlay.cbActionMask=(m_bResponse[wChiarID]==false)?m_cbUserAction[wChiarID]:WIK_NULL;

			//历史记录
			StatusPlay.wOutCardUser=m_wOutCardUser;
			StatusPlay.cbOutCardData=m_cbOutCardData;
			for (int i = 0;i<GAME_PLAYER;i++)
			{
				CopyMemory(StatusPlay.cbDiscardCard[i],m_cbDiscardCard[i],sizeof(BYTE)*60);
			}
			CopyMemory(StatusPlay.cbDiscardCount,m_cbDiscardCount,sizeof(StatusPlay.cbDiscardCount));

			//组合扑克
			CopyMemory(StatusPlay.WeaveItemArray,m_WeaveItemArray,sizeof(m_WeaveItemArray));
			CopyMemory(StatusPlay.cbWeaveCount,m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));


			//扑克数据
			StatusPlay.cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex[wChiarID],StatusPlay.cbCardData);
			StatusPlay.cbSendCardData=((m_cbSendCardData!=0)&&(m_wProvideUser==wChiarID))?m_cbSendCardData:0x00;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_TIMER_XIAO_HU:  //换牌结束
		{
			ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
			m_pITableFrame->SetGameStatus(GS_MJ_PLAY);

			m_bSendStatus = true;
			DispatchCardData(m_wCurrentUser);
			return true;
		}
	}
	return false;
}

//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID* pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_OUT_CARD:		//出牌消息
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_OutCard));
			if (wDataSize!=sizeof(CMD_C_OutCard)) return false;

			//用户效验
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			return OnUserOutCard(pIServerUserItem->GetChairID(),pOutCard->cbCardData);
		}
	case SUB_C_OPERATE_CARD:	//操作消息
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_OperateCard));
			if (wDataSize!=sizeof(CMD_C_OperateCard)) return false;

			//用户效验
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			CMD_C_OperateCard * pOperateCard=(CMD_C_OperateCard *)pDataBuffer;
			return OnUserOperateCard(pIServerUserItem->GetChairID(),pOperateCard->cbOperateCode,pOperateCard->cbOperateCard);
		}
	case SUB_C_TRUSTEE:
		{
			CMD_C_Trustee *pTrustee =(CMD_C_Trustee *)pDataBuffer;
			if(wDataSize != sizeof(CMD_C_Trustee)) return false;


			m_bTrustee[pIServerUserItem->GetChairID()]=pTrustee->bTrustee;
			CMD_S_Trustee Trustee;
			Trustee.bTrustee=pTrustee->bTrustee;
			Trustee.wChairID = pIServerUserItem->GetChairID();
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));

			return true;
		}
	case SUB_C_XIAOHU:
		{
			if (m_pITableFrame->GetGameStatus() != GS_MJ_XIAOHU)
			{
				return true;
			}
			WORD wChairID = pIServerUserItem->GetChairID();
			m_dwChiHuKind[wChairID] = AnalyseChiHuCardCS_XIAOHU(m_cbCardIndex[wChairID],m_StartHuRight[wChairID]);

			LONGLONG lStartHuScore = 0;
			int wFanShu = m_GameLogic.GetChiHuActionRank_CS(m_StartHuRight[wChairID]);
			lStartHuScore  = wFanShu*m_pGameServiceOption->lCellScore;
			m_lStartHuScore[wChairID] += lStartHuScore*3;

			for (int i=0;i<GAME_PLAYER;i++)
			{
				if (i == wChairID)continue;
				m_lStartHuScore[i] -= lStartHuScore;
			}

			m_cbUserAction[wChairID] = WIK_NULL;

			bool bEnd = true;
			for (int i = 0;i < GAME_PLAYER;i++)
			{
				if (m_cbUserAction[i] != WIK_NULL)
				{
					bEnd = false;
				}
			}
			if (bEnd)
			{
				ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
				m_pITableFrame->SetGameStatus(GS_MJ_PLAY);
				m_bSendStatus = true;
				DispatchCardData(m_wCurrentUser);
				m_pITableFrame->KillGameTimer(IDI_TIMER_XIAO_HU);

			}
			return true;
		}
	}

	return false;
}

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

//用户起来
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//庄家设置
	if (bLookonUser==false)
	{
		m_bTrustee[wChairID]=false;
		CMD_S_Trustee Trustee;
		Trustee.bTrustee=false;
		Trustee.wChairID = wChairID;
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
	}

	return true;
}
void CTableFrameSink::SetPrivateInfo(BYTE bGameTypeIdex,DWORD	bGameRuleIdex)
{
	m_cbGameTypeIdex = bGameTypeIdex;
	m_dwGameRuleIdex = bGameRuleIdex;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY) return true;

	//错误断言
	ASSERT(wChairID==m_wCurrentUser);
	ASSERT(m_GameLogic.IsValidCard(cbCardData)==true);

	//效验参数
	if (wChairID!=m_wCurrentUser) return true;
	if (m_GameLogic.IsValidCard(cbCardData)==false) return true;

	//删除扑克
	if (m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],cbCardData)==false)
	{
		ASSERT(FALSE);
		return true;
	}

	//设置变量
	m_bSendStatus=true;
	if( m_bGangStatus )
	{
		m_bGangStatus = false;
		m_bGangOutStatus = true;
	}
	m_cbUserAction[wChairID]=WIK_NULL;
	m_cbPerformAction[wChairID]=WIK_NULL;

	//出牌记录
	m_cbOutCardCount++;
	m_wOutCardUser=wChairID;
	m_cbOutCardData=cbCardData;

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbOutCardData=cbCardData;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));

	m_wProvideUser=wChairID;
	m_cbProvideCard=cbCardData;

	//用户切换
	m_wCurrentUser=(wChairID+GAME_PLAYER-1)%GAME_PLAYER;

	//响应判断
	bool bAroseAction=EstimateUserRespond(wChairID,cbCardData,EstimatKind_OutCard);

	//派发扑克
	if (bAroseAction==false) DispatchCardData(m_wCurrentUser);

	return true;
}

//用户操作
bool CTableFrameSink::OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()!=GS_MJ_FREE);
	if (m_pITableFrame->GetGameStatus()==GS_MJ_FREE)
		return true;

	//效验用户	注意：机器人有可能发生此断言
	//ASSERT((wChairID==m_wCurrentUser)||(m_wCurrentUser==INVALID_CHAIR));
	if ((wChairID!=m_wCurrentUser)&&(m_wCurrentUser!=INVALID_CHAIR)) 
		return true;

	//被动动作
	if (m_wCurrentUser==INVALID_CHAIR)
	{
		//效验状态
		if (m_bResponse[wChairID]==true) 
			return true;
		if ((cbOperateCode!=WIK_NULL)&&((m_cbUserAction[wChairID]&cbOperateCode)==0))
			return true;

		//变量定义
		WORD wTargetUser=wChairID;
		BYTE cbTargetAction=cbOperateCode;

		//设置变量
		m_bResponse[wChairID]=true;
		m_cbPerformAction[wChairID]=cbOperateCode;
		m_cbOperateCard[wChairID]=m_cbProvideCard;

		//执行判断
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//获取动作
			BYTE cbUserAction=(m_bResponse[i]==false)?m_cbUserAction[i]:m_cbPerformAction[i];

			//优先级别
			BYTE cbUserActionRank=m_GameLogic.GetUserActionRank(cbUserAction);
			BYTE cbTargetActionRank=m_GameLogic.GetUserActionRank(cbTargetAction);

			//动作判断
			if (cbUserActionRank>cbTargetActionRank)
			{
				wTargetUser=i;
				cbTargetAction=cbUserAction;
			}
		}
		if (m_bResponse[wTargetUser]==false) 
			return true;

		//吃胡等待
		if (cbTargetAction==WIK_CHI_HU)
		{
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if ((m_bResponse[i]==false)&&(m_cbUserAction[i]&WIK_CHI_HU))
					return true;
			}
		}

		//放弃操作
		if (cbTargetAction==WIK_NULL)
		{
			//用户状态
			ZeroMemory(m_bResponse,sizeof(m_bResponse));
			ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
			ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
			ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

			//发送扑克
			DispatchCardData(m_wResumeUser);

			return true;
		}

		//变量定义
		BYTE cbTargetCard=m_cbOperateCard[wTargetUser];

		//出牌变量
		m_cbOutCardData=0;
		m_bSendStatus=true;
		m_wOutCardUser=INVALID_CHAIR;

		//胡牌操作
		if (cbTargetAction==WIK_CHI_HU)
		{
			setNiaoCard();//结束后设置鸟牌
			//结束信息
			m_cbChiHuCard=cbTargetCard;

			for (WORD i=(m_wProvideUser+GAME_PLAYER-1)%GAME_PLAYER;i!=m_wProvideUser;i = (i+GAME_PLAYER-1)%GAME_PLAYER)
			{
				//过虑判断
				if ((m_cbPerformAction[i]&WIK_CHI_HU)==0)
					continue;

				//胡牌判断
				BYTE cbWeaveItemCount=m_cbWeaveItemCount[i];
				tagWeaveItem * pWeaveItem=m_WeaveItemArray[i];
				m_dwChiHuKind[i] = AnalyseChiHuCard(m_cbCardIndex[i],pWeaveItem,cbWeaveItemCount,m_cbChiHuCard,m_ChiHuRight[i]);

				//插入扑克
				if (m_dwChiHuKind[i]!=WIK_NULL) 
				{
					m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbChiHuCard)]++;
					ProcessChiHuUser( i,false );
				}
			}

			OnEventGameConclude( INVALID_CHAIR,NULL,GER_NORMAL );

			return true;
		}

		//用户状态
		ZeroMemory(m_bResponse,sizeof(m_bResponse));
		ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
		ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
		ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

		//组合扑克
		ASSERT(m_cbWeaveItemCount[wTargetUser]<4);
		WORD wIndex=m_cbWeaveItemCount[wTargetUser]++;
		m_WeaveItemArray[wTargetUser][wIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wTargetUser][wIndex].cbCenterCard=cbTargetCard;
		m_WeaveItemArray[wTargetUser][wIndex].cbWeaveKind=cbTargetAction;
		m_WeaveItemArray[wTargetUser][wIndex].wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;

		//删除扑克
		switch (cbTargetAction)
		{
		case WIK_LEFT:		//上牌操作
		{
			//删除扑克
			BYTE cbRemoveCard[3];
			m_GameLogic.GetWeaveCard(WIK_LEFT,cbTargetCard,cbRemoveCard);
			VERIFY( m_GameLogic.RemoveCard(cbRemoveCard,3,&cbTargetCard,1) );
			VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );

			break;
		}
		case WIK_RIGHT:		//上牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[3];
				m_GameLogic.GetWeaveCard(WIK_RIGHT,cbTargetCard,cbRemoveCard);
				VERIFY( m_GameLogic.RemoveCard(cbRemoveCard,3,&cbTargetCard,1) );
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );

				break;
			}
		case WIK_CENTER:	//上牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[3];
				m_GameLogic.GetWeaveCard(WIK_CENTER,cbTargetCard,cbRemoveCard);
				VERIFY( m_GameLogic.RemoveCard(cbRemoveCard,3,&cbTargetCard,1) );
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );

				break;
			}
		case WIK_PENG:		//碰牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard};
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );

				break;
			}
		case WIK_GANG:		//杠牌操作
			{
				//删除扑克,被动动作只存在放杠
				BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard,cbTargetCard};
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard)) );

				break;
			}
		default:
			ASSERT( FALSE );
			return false;
		}

		//构造结果
		CMD_S_OperateResult OperateResult;
		OperateResult.wOperateUser=wTargetUser;
		OperateResult.cbOperateCard=cbTargetCard;
		OperateResult.cbOperateCode=cbTargetAction;
		OperateResult.wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));

		//设置用户
		m_wCurrentUser=wTargetUser;

		//杠牌处理
		if (cbTargetAction==WIK_GANG)
		{
			//发送信息
			CMD_S_GangScore gs;
			ZeroMemory( &gs,sizeof(gs) );
			gs.wChairId = wTargetUser;
			gs.cbXiaYu = FALSE;
			gs.lGangScore[wTargetUser] = m_pGameServiceOption->lCellScore*2L;
			gs.lGangScore[m_wProvideUser] = -m_pGameServiceOption->lCellScore*2L;
			m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_GANG_SCORE,&gs,sizeof(gs) );
			m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_GANG_SCORE,&gs,sizeof(gs) );

			//杠得分
			BYTE cbIndex = m_GangScore[wTargetUser].cbGangCount++;
			m_GangScore[wTargetUser].lScore[cbIndex][wTargetUser] = m_pGameServiceOption->lCellScore*2;
			m_GangScore[wTargetUser].lScore[cbIndex][m_wProvideUser] = -m_pGameServiceOption->lCellScore*2;
			m_bGangStatus = true;
			DispatchCardData(wTargetUser,true);
		}

		return true;
	}

	//主动动作
	if (m_wCurrentUser==wChairID)
	{
		//效验操作
		if ((cbOperateCode==WIK_NULL)||((m_cbUserAction[wChairID]&cbOperateCode)==0))
			return true;

		//扑克效验
		ASSERT((cbOperateCode==WIK_NULL)||(cbOperateCode==WIK_CHI_HU)||
			(m_GameLogic.IsValidCard(cbOperateCard)==true));
		if ((cbOperateCode!=WIK_NULL)&&(cbOperateCode!=WIK_CHI_HU)
			&&(m_GameLogic.IsValidCard(cbOperateCard)==false)) 
			return true;

		//设置变量
		m_bSendStatus=true;
		m_cbUserAction[m_wCurrentUser]=WIK_NULL;
		m_cbPerformAction[m_wCurrentUser]=WIK_NULL;

		bool bPublic=false;

		//执行动作
		switch (cbOperateCode)
		{
		case WIK_GANG:			//杠牌操作
			{
				//变量定义
				BYTE cbWeaveIndex=0xFF;
				BYTE cbCardIndex=m_GameLogic.SwitchToCardIndex(cbOperateCard);

				//杠牌处理
				if (m_cbCardIndex[wChairID][cbCardIndex]==1)
				{
					//寻找组合
					for (BYTE i=0;i<m_cbWeaveItemCount[wChairID];i++)
					{
						BYTE cbWeaveKind=m_WeaveItemArray[wChairID][i].cbWeaveKind;
						BYTE cbCenterCard=m_WeaveItemArray[wChairID][i].cbCenterCard;
						if ((cbCenterCard==cbOperateCard)&&(cbWeaveKind==WIK_PENG))
						{
							bPublic=true;
							cbWeaveIndex=i;
							break;
						}
					}

					//效验动作
					ASSERT(cbWeaveIndex!=0xFF);
					if (cbWeaveIndex==0xFF) return false;

					//组合扑克
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=TRUE;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind=cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard;
				}
				else
				{
					//扑克效验
					ASSERT(m_cbCardIndex[wChairID][cbCardIndex]==4);
					if (m_cbCardIndex[wChairID][cbCardIndex]!=4) 
						return false;

					//设置变量
					bPublic=false;
					cbWeaveIndex=m_cbWeaveItemCount[wChairID]++;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=FALSE;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind=cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard;
				}

				//删除扑克
				m_cbCardIndex[wChairID][cbCardIndex]=0;

				m_bGangStatus = true;

				//杠牌得分
				LONGLONG lScore = bPublic?m_pGameServiceOption->lCellScore:m_pGameServiceOption->lCellScore*2L;
				BYTE cbGangIndex = m_GangScore[wChairID].cbGangCount++;
				for( WORD i = 0; i < GAME_PLAYER; i++ )
				{
					if(  i == wChairID ) continue;

					m_GangScore[wChairID].lScore[cbGangIndex][i] = -lScore;
					m_GangScore[wChairID].lScore[cbGangIndex][wChairID] += lScore;
				}

				//构造结果
				CMD_S_OperateResult OperateResult;
				OperateResult.wOperateUser=wChairID;
				OperateResult.wProvideUser=wChairID;
				OperateResult.cbOperateCode=cbOperateCode;
				OperateResult.cbOperateCard=cbOperateCard;

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));


				//发送信息
				CMD_S_GangScore gs;
				gs.wChairId = wChairID;
				ZeroMemory( &gs,sizeof(gs) );
				gs.cbXiaYu = bPublic?FALSE:TRUE;
				for( WORD i = 0; i < GAME_PLAYER; i++ )
				{
					if( i == wChairID ) continue;

					gs.lGangScore[i] = -lScore;
					gs.lGangScore[wChairID] += lScore;
				}
				m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_GANG_SCORE,&gs,sizeof(gs) );
				m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_GANG_SCORE,&gs,sizeof(gs) );

				//效验动作
				bool bAroseAction=false;
				if (bPublic==true) bAroseAction=EstimateUserRespond(wChairID,cbOperateCard,EstimatKind_GangCard);

				//发送扑克
				if (bAroseAction==false)
				{
					DispatchCardData(wChairID,true);
				}
				return true;
			}
		case WIK_CHI_HU:		//吃胡操作
			{
				setNiaoCard();//结束后设置鸟牌
				//吃牌权位
				if (m_cbOutCardCount==0)
				{
					m_wProvideUser = m_wCurrentUser;
					m_cbProvideCard = m_cbSendCardData;
				}

				//普通胡牌
				BYTE cbWeaveItemCount=m_cbWeaveItemCount[wChairID];
				tagWeaveItem * pWeaveItem=m_WeaveItemArray[wChairID];
				m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],&m_cbProvideCard,1);
				m_dwChiHuKind[wChairID] = AnalyseChiHuCard(m_cbCardIndex[wChairID],pWeaveItem,cbWeaveItemCount,m_cbProvideCard,
					m_ChiHuRight[wChairID]);
				m_cbCardIndex[wChairID][m_GameLogic.SwitchToCardIndex(m_cbProvideCard)]++;

				//结束信息
				m_cbChiHuCard=m_cbProvideCard;

				ProcessChiHuUser( wChairID,false );

				
				OnEventGameConclude( INVALID_CHAIR,NULL,GER_NORMAL );

				return true;
			}
		}

		return true;
	}

	return false;
}

//发送操作
bool CTableFrameSink::SendOperateNotify()
{
	//发送提示
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_cbUserAction[i]!=WIK_NULL)
		{
			//构造数据
			CMD_S_OperateNotify OperateNotify;
			OperateNotify.wResumeUser=m_wResumeUser;
			OperateNotify.cbActionCard=m_cbProvideCard;
			OperateNotify.cbActionMask=m_cbUserAction[i];

			//发送数据
			m_pITableFrame->SendTableData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
			m_pITableFrame->SendLookonData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
		}
	}

	return true;
}

//派发扑克
bool CTableFrameSink::DispatchCardData(WORD wCurrentUser,bool bTail)
{
	//状态效验
	ASSERT(wCurrentUser!=INVALID_CHAIR);
	if (wCurrentUser==INVALID_CHAIR)
		return false;

	//丢弃扑克
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		m_cbDiscardCount[m_wOutCardUser]++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]-1]=m_cbOutCardData;
	}

	//荒庄结束
	if (m_cbLeftCardCount==0)
	{
		m_cbChiHuCard=0;
		m_wProvideUser=INVALID_CHAIR;
		OnEventGameConclude(m_wProvideUser,NULL,GER_NORMAL);

		return true;
	}

	//设置变量
	m_cbOutCardData=0;
	m_wCurrentUser=wCurrentUser;
	m_wOutCardUser=INVALID_CHAIR;

	//杠后炮
	if( m_bGangOutStatus )
	{
		m_bGangOutStatus = false;
	}

	//发牌处理
	if (m_bSendStatus==true)
	{
		//发送扑克
		m_cbSendCardCount++;
		if (hasRule(GAME_TYPE_ZZ_HONGZHONG))
		{
			m_cbSendCardData=m_cbRepertoryCard_HZ[--m_cbLeftCardCount];
		}
		else
		{
			m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];
		}

		//胡牌判断
		CChiHuRight chr;
		m_wProvideUser = wCurrentUser;
		m_cbUserAction[wCurrentUser]|= AnalyseChiHuCard(m_cbCardIndex[wCurrentUser],m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],m_cbSendCardData,chr);

		//加牌
		m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

		//设置变量
		m_cbProvideCard = m_cbSendCardData;

		//杠牌判断
		if (m_cbLeftCardCount>0)
		{
			tagGangCardResult GangCardResult;
			m_cbUserAction[wCurrentUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[wCurrentUser],
				m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],GangCardResult);
		}
	}


	//构造数据
	CMD_S_SendCard SendCard;
	SendCard.wCurrentUser=wCurrentUser;
	SendCard.bTail = bTail;
	SendCard.cbActionMask=m_cbUserAction[wCurrentUser];
	SendCard.cbCardData=(m_bSendStatus==true)?m_cbSendCardData:0x00;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

	return true;
}

//响应判断
bool CTableFrameSink::EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind)
{
	//变量定义
	bool bAroseAction=false;

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//动作判断
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//用户过滤
		if (wCenterUser==i) continue;

		//出牌类型
		if (EstimatKind==EstimatKind_OutCard)
		{
			////碰牌判断
			m_cbUserAction[i]|=m_GameLogic.EstimatePengCard(m_cbCardIndex[i],cbCenterCard);

			//杠牌判断
			if (m_cbLeftCardCount>0) 
			{
				m_cbUserAction[i]|=m_GameLogic.EstimateGangCard(m_cbCardIndex[i],cbCenterCard);
			}
		}

		//吃胡判断
		CChiHuRight chr;
		BYTE cbWeaveCount=m_cbWeaveItemCount[i];
		m_cbUserAction[i]|= AnalyseChiHuCard(m_cbCardIndex[i],m_WeaveItemArray[i],cbWeaveCount,cbCenterCard,chr);
		

		//结果判断
		if (m_cbUserAction[i]!=WIK_NULL) 
			bAroseAction=true;
	}

	//长沙麻将吃操作
	if (m_cbGameTypeIdex == GAME_TYPE_CS)
	{
		m_cbUserAction[m_wCurrentUser] = m_GameLogic.EstimateEatCard(m_cbCardIndex[m_wCurrentUser],cbCenterCard);

		//结果判断
		if (m_cbUserAction[m_wCurrentUser]!=WIK_NULL) 
			bAroseAction=true;
	}
	//结果处理
	if (bAroseAction==true) 
	{
		//设置变量
		m_wProvideUser=wCenterUser;
		m_cbProvideCard=cbCenterCard;
		m_wResumeUser=m_wCurrentUser;
		m_wCurrentUser=INVALID_CHAIR;

		//发送提示
		SendOperateNotify();
		return true;
	}

	return false;
}
//
void CTableFrameSink::ProcessChiHuUser( WORD wChairId, bool bGiveUp )
{
	if( !bGiveUp )
	{
		//引用权位
		CChiHuRight &chr = m_ChiHuRight[wChairId];

		FiltrateRight( wChairId,chr );
		WORD wFanShu = 0;
		if ( m_cbGameTypeIdex == GAME_TYPE_ZZ)
		{
			wFanShu = m_GameLogic.GetChiHuActionRank_ZZ(chr);
		}
		if ( m_cbGameTypeIdex == GAME_TYPE_CS)
		{
			wFanShu = m_GameLogic.GetChiHuActionRank_CS(chr);
		}
		
		LONGLONG lChiHuScore = wFanShu*m_pGameServiceOption->lCellScore;
		
		//杠上炮,呼叫转移
		if( !(chr&CHR_GANG_SHANG_PAO).IsEmpty() )
		{
			BYTE cbGangIndex = m_GangScore[m_wProvideUser].cbGangCount-1;
			//一炮多响的情况下,胡牌者平分杠得分
			BYTE cbChiHuCount = 0;
			for( WORD i = 0; i < GAME_PLAYER; i++ )
				if( m_cbPerformAction[i]==WIK_CHI_HU ) cbChiHuCount++;
			if( cbChiHuCount == 1 )
			{
				LONGLONG lScore = m_GangScore[m_wProvideUser].lScore[cbGangIndex][wChairId];
				m_GangScore[m_wProvideUser].lScore[cbGangIndex][wChairId] = m_GangScore[m_wProvideUser].lScore[cbGangIndex][m_wProvideUser];
				m_GangScore[m_wProvideUser].lScore[cbGangIndex][m_wProvideUser] = lScore;
			}
			else
			{
				LONGLONG lGangScore = m_GangScore[m_wProvideUser].lScore[cbGangIndex][m_wProvideUser]/cbChiHuCount;
				lGangScore = __max(lGangScore,m_pGameServiceOption->lCellScore);
				for( WORD i = 0; i < GAME_PLAYER; i++ )
				{
					if( m_cbPerformAction[i]==WIK_CHI_HU )
						m_GangScore[m_wProvideUser].lScore[cbGangIndex][i] = lGangScore;
				}
				m_GangScore[m_wProvideUser].lScore[cbGangIndex][m_wProvideUser] = 0;
				for( WORD i = 0; i < GAME_PLAYER; i++ )
				{
					if( i != m_wProvideUser)
						m_GangScore[m_wProvideUser].lScore[cbGangIndex][m_wProvideUser] += m_GangScore[m_wProvideUser].lScore[cbGangIndex][i];
				}
			}
		}
		//抢杠杠分不算
		else if( !(chr&CHR_QIANG_GANG_HU).IsEmpty() )
		{
			m_GangScore[m_wProvideUser].cbGangCount--;
		}

		if( m_wProvideUser != wChairId )
			m_wLostFanShu[m_wProvideUser][wChairId] = (WORD)lChiHuScore;
		else
		{
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				if( i == wChairId ) continue;

				m_wLostFanShu[i][wChairId] = (WORD)lChiHuScore;
			}
		}

		if( m_wProvideUser == wChairId )
		{
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				if( i == wChairId ) continue;

				//胡牌分
				m_lGameScore[i] -= lChiHuScore;
				m_lGameScore[wChairId] += lChiHuScore;

				//中鸟分
				m_lGameScore[i] -= m_cbNiaoPick* m_pGameServiceOption->lCellScore;
				m_lGameScore[wChairId] += m_cbNiaoPick* m_pGameServiceOption->lCellScore;
			}
		}
		//点炮
		else
		{

			m_lGameScore[m_wProvideUser] -= lChiHuScore;
			m_lGameScore[wChairId] += lChiHuScore;

			m_lGameScore[m_wProvideUser] -= m_cbNiaoPick* m_pGameServiceOption->lCellScore;
			m_lGameScore[wChairId] += m_cbNiaoPick* m_pGameServiceOption->lCellScore;
		}

		//设置变量
		m_wProvider[wChairId] = m_wProvideUser;
		m_bGangStatus = false;
		m_bGangOutStatus = false;

		//发送消息
		CMD_S_ChiHu ChiHu;
		ChiHu.wChiHuUser = wChairId;
		ChiHu.wProviderUser = m_wProvideUser;
		ChiHu.lGameScore = m_lGameScore[wChairId];
		ChiHu.cbCardCount = m_GameLogic.GetCardCount( m_cbCardIndex[wChairId] );
		ChiHu.cbChiHuCard = m_cbProvideCard;
		m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_CHI_HU,&ChiHu,sizeof(ChiHu) );
		m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_CHI_HU,&ChiHu,sizeof(ChiHu) );

	}

	return;
}

BYTE CTableFrameSink::GetNiaoCardNum()
{
	BYTE nNum = ZZ_ZHANIAO0;
	if(hasRule(GAME_TYPE_ZZ_ZHANIAO2))
	{
		nNum = ZZ_ZHANIAO2;
	}
	if(hasRule(GAME_TYPE_ZZ_ZHANIAO4))
	{
		nNum = ZZ_ZHANIAO4;
	}
	if(hasRule(GAME_TYPE_ZZ_ZHANIAO6))
	{
		nNum = ZZ_ZHANIAO6;
	}

	if (nNum>m_cbLeftCardCount)
	{
		nNum = m_cbLeftCardCount;
	}
	return nNum;
}

void CTableFrameSink::setNiaoCard()
{
	ZeroMemory(m_cbCardDataNiao,sizeof(m_cbCardDataNiao));
	m_cbNiaoCount = GetNiaoCardNum();

	if (m_cbNiaoCount>ZZ_ZHANIAO0)
	{
		BYTE cbCardIndexTemp[MAX_INDEX];
		m_cbLeftCardCount-= m_cbNiaoCount;
		if (hasRule(GAME_TYPE_ZZ_HONGZHONG))
		{
			m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard_HZ[m_cbLeftCardCount],m_cbNiaoCount,cbCardIndexTemp);
		}
		else
		{
			m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard[m_cbLeftCardCount],m_cbNiaoCount,cbCardIndexTemp);
		}
		m_GameLogic.SwitchToCardData(cbCardIndexTemp,m_cbCardDataNiao);
	}
	m_cbNiaoPick = m_GameLogic.GetPickNiaoCount(m_cbCardDataNiao,m_cbNiaoCount);
}


//////////////////////////////////////////////////////////////////////////
