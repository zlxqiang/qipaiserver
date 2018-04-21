#ifndef CMD_SPARROW_HEAD_FILE
#define CMD_SPARROW_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义
#pragma pack(1)

#define KIND_ID						310									//游戏 I D

//组件属性
#define GAME_PLAYER					4									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(6,0,3)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				//程序版本

#define GAME_NAME					TEXT("湖南麻将")					//游戏名字
#define GAME_GENRE					(GAME_GENRE_SCORE|GAME_GENRE_MATCH|GAME_GENRE_GOLD)	//游戏类型

//游戏状态
#define GS_MJ_FREE					GAME_STATUS_FREE								//空闲状态
#define GS_MJ_PLAY					(GAME_STATUS_PLAY+1)							//游戏状态
#define GS_MJ_XIAOHU				(GAME_STATUS_PLAY+2)							//小胡状态

//常量定义
#define MAX_WEAVE					4									//最大组合
#define MAX_INDEX					34									//最大索引
#define MAX_COUNT					14									//最大数目
#define MAX_REPERTORY				108									//最大库存
#define MAX_REPERTORY_HZ			112									//红中麻将最大库存

#define MAX_NIAO_CARD             6									//最大中鸟数


#define MAX_RIGHT_COUNT				1									//最大权位DWORD个数	

#define GAME_TYPE_ZZ				0
#define GAME_TYPE_CS				1

#define GAME_TYPE_ZZ_258			0		//只能258做将
#define GAME_TYPE_ZZ_ZIMOHU		    1		//只能自模胡
#define GAME_TYPE_ZZ_QIDUI			2		//可胡七对
#define GAME_TYPE_ZZ_QIANGGANGHU	3		//可抢杠胡
#define GAME_TYPE_ZZ_ZHANIAO2		4		//扎鸟2个
#define GAME_TYPE_ZZ_ZHANIAO4		5		//扎鸟4个
#define GAME_TYPE_ZZ_ZHANIAO6		6		//扎鸟6个
#define GAME_TYPE_ZZ_HONGZHONG		7		//红中玩法


#define ZZ_ZHANIAO0		0		//扎鸟2个
#define ZZ_ZHANIAO2		2		//扎鸟2个
#define ZZ_ZHANIAO4		4		//扎鸟4个
#define ZZ_ZHANIAO6		6		//扎鸟6个
//////////////////////////////////////////////////////////////////////////

//组合子项
struct CMD_WeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_OUT_CARD				101									//出牌命令
#define SUB_S_SEND_CARD				102									//发送扑克
#define SUB_S_OPERATE_NOTIFY		104									//操作提示
#define SUB_S_OPERATE_RESULT		105									//操作命令
#define SUB_S_GAME_END				106									//游戏结束
#define SUB_S_TRUSTEE				107									//用户托管
#define SUB_S_CHI_HU				108									//
#define SUB_S_GANG_SCORE			110									//

//游戏状态
struct CMD_S_StatusFree
{
	LONGLONG						lCellScore;							//基础金币
	WORD							wBankerUser;						//庄家用户
	bool							bTrustee[GAME_PLAYER];				//是否托管
};

//游戏状态
struct CMD_S_StatusPlay
{
	//游戏变量
	LONGLONG						lCellScore;									//单元积分
	WORD							wBankerUser;								//庄家用户
	WORD							wCurrentUser;								//当前用户

	//状态变量
	BYTE							cbActionCard;								//动作扑克
	BYTE							cbActionMask;								//动作掩码
	BYTE							cbLeftCardCount;							//剩余数目
	bool							bTrustee[GAME_PLAYER];						//是否托管
	WORD							wWinOrder[GAME_PLAYER];						//

	//出牌信息
	WORD							wOutCardUser;								//出牌用户
	BYTE							cbOutCardData;								//出牌扑克
	BYTE							cbDiscardCount[GAME_PLAYER];				//丢弃数目
	BYTE							cbDiscardCard[GAME_PLAYER][60];				//丢弃记录

	//扑克数据
	BYTE							cbCardCount;								//扑克数目
	BYTE							cbCardData[MAX_COUNT];						//扑克列表
	BYTE							cbSendCardData;								//发送扑克

	//组合扑克
	BYTE							cbWeaveCount[GAME_PLAYER];					//组合数目
	CMD_WeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//组合扑克
};

//游戏开始
struct CMD_S_GameStart
{
	LONG							lSiceCount;									//骰子点数
	WORD							wBankerUser;								//庄家用户
	WORD							wCurrentUser;								//当前用户
	BYTE							cbUserAction;								//用户动作
	BYTE							cbCardData[MAX_COUNT*GAME_PLAYER];			//扑克列表
	BYTE							cbLeftCardCount;							//
	BYTE                         cbXiaoHuTag;                           //小胡标记 0 没小胡 1 有小胡；

};

//出牌命令
struct CMD_S_OutCard
{
	WORD							wOutCardUser;						//出牌用户
	BYTE							cbOutCardData;						//出牌扑克
};

//发送扑克
struct CMD_S_SendCard
{
	BYTE							cbCardData;							//扑克数据
	BYTE							cbActionMask;						//动作掩码
	WORD							wCurrentUser;						//当前用户
	bool							bTail;								//末尾发牌
};


//操作提示
struct CMD_S_OperateNotify
{
	WORD							wResumeUser;						//还原用户
	BYTE							cbActionMask;						//动作掩码
	BYTE							cbActionCard;						//动作扑克
};

//操作命令
struct CMD_S_OperateResult
{
	WORD							wOperateUser;						//操作用户
	WORD							wProvideUser;						//供应用户
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard;						//操作扑克
};

//游戏结束
struct CMD_S_GameEnd
{
	BYTE							cbCardCount[GAME_PLAYER];			//
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];	//
	//结束信息
	WORD							wProvideUser[GAME_PLAYER];			//供应用户
	DWORD							dwChiHuRight[GAME_PLAYER];			//胡牌类型
	DWORD							dwStartHuRight[GAME_PLAYER];			//起手胡牌类型
	LONGLONG						lStartHuScore[GAME_PLAYER];			//起手胡牌分数

	//积分信息
	LONGLONG						lGameScore[GAME_PLAYER];			//游戏积分
	int								lGameTax[GAME_PLAYER];				//

	WORD							wWinOrder[GAME_PLAYER];				//胡牌排名

	LONGLONG						lGangScore[GAME_PLAYER];//详细得分
	BYTE							cbGenCount[GAME_PLAYER];			//
	WORD							wLostFanShu[GAME_PLAYER][GAME_PLAYER];
	WORD							wLeftUser;	//

	//组合扑克
	BYTE							cbWeaveCount[GAME_PLAYER];					//组合数目
	CMD_WeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//组合扑克


	BYTE							cbCardDataNiao[MAX_NIAO_CARD];	// 鸟牌
	BYTE							cbNiaoCount;	//鸟牌个数
	BYTE							cbNiaoPick;	//中鸟个数
};

//用户托管
struct CMD_S_Trustee
{
	bool							bTrustee;							//是否托管
	WORD							wChairID;							//托管用户
};

//
struct CMD_S_ChiHu
{
	WORD							wChiHuUser;							//
	WORD							wProviderUser;						//
	BYTE							cbChiHuCard;						//
	BYTE							cbCardCount;						//
	LONGLONG						lGameScore;							//
	BYTE							cbWinOrder;							//
};

//
struct CMD_S_GangScore
{
	WORD							wChairId;							//
	BYTE							cbXiaYu;							//
	LONGLONG						lGangScore[GAME_PLAYER];			//
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_OUT_CARD				1									//出牌命令
#define SUB_C_OPERATE_CARD			3									//操作扑克
#define SUB_C_TRUSTEE				4									//用户托管
#define SUB_C_XIAOHU				5									//小胡

//出牌命令
struct CMD_C_OutCard
{
	BYTE							cbCardData;							//扑克数据
};

//操作命令
struct CMD_C_OperateCard
{
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard;						//操作扑克
};

//用户托管
struct CMD_C_Trustee
{
	bool							bTrustee;							//是否托管	
};

//起手小胡
struct CMD_C_XiaoHu
{
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard;						//操作扑克
};


//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
