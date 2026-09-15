#include <nitro.h>

#include "contest_move.h"

#include "assert.h"
#include "common.h"
#include "fs/msgdata/contest_effects.h"

#define FIRST_CONTEST_EFFECT_DESCRIPTION_ID  CONTEST_EFFECTS_00046

// Only appealPoints is used
typedef struct {
	u16  lineOneEffectMessageID;
	u16  lineTwoEffectMessageID;
	s8   appealPoints;
	u16  effectIDA;
	u8   effectTagA;
	u16  effectIDB;
	u8   effectTagB;
	u16  effectIDC;
	u8   effectTagC;
	u16  effectIDD;
	u8   effectTagD;
	u16  effectIDE;
	u8   effectTagE;
} ContestMoveEffectData;


static const ContestMoveEffectData sContestMoves[] = {
	[CONTEST_EFFECT_NONE] = {
		0,
		0,
		0
	},
	[CONTEST_EFFECT_FIRST_NEXT_TURN] = {
		CONTEST_EFFECTS_PerformFirst,
		CONTEST_EFFECTS_NextTurn1,
		20,
		0,
		2,
		1,
		7,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_LAST_NEXT_TURN] = {
		CONTEST_EFFECTS_PerformLast,
		CONTEST_EFFECTS_NextTurn2,
		20,
		2,
		2,
		3,
		7,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_DOUBLED_JUDGE] = {
		CONTEST_EFFECTS_EachDoubled,
		CONTEST_EFFECTS_JudgePlusTwo,
		0,
		4,
		2,
		5,
		2,
		6,
		2,
		7,
		2,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_2_HEARTS_WHEN_VOLTAGE_UP] = {
		CONTEST_EFFECTS_IfTheVoltage,
		CONTEST_EFFECTS_GoesUpPlusTwo,
		20,
		8,
		8,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_BASIC] = {
		CONTEST_EFFECTS_BasicAct,
		CONTEST_EFFECTS_Unused,
		30,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_UNIQUE_JUDGE] = {
		CONTEST_EFFECTS_IfJudgesAre,
		CONTEST_EFFECTS_NotDoubledPlusThree,
		10,
		9,
		2,
		10,
		2,
		0xFFFF,
		0,
		0xFFFF,
		0,
		11,
		2
	},
	[CONTEST_EFFECT_CONSECUTIVE_USE] = {
		CONTEST_EFFECTS_PerformableTwo,
		CONTEST_EFFECTS_TurnsInARow,
		20,
		12,
		2,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_VOLTAGE] = {
		CONTEST_EFFECTS_VoltagePts,
		CONTEST_EFFECTS_AreAdded,
		0,
		13,
		5,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_ALL_SAME_JUDGE] = {
		CONTEST_EFFECTS_IfAllChoose,
		CONTEST_EFFECTS_SameJudgePlusFifteen,
		0,
		14,
		2,
		15,
		2,
		0xFFFF,
		0,
		0xFFFF,
		0,
		16,
		2
	},
	[CONTEST_EFFECT_LOWERS_VOLTAGE] = {
		CONTEST_EFFECTS_LowersVoltage,
		CONTEST_EFFECTS_OfJudgesByOne,
		20,
		17,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_DOUBLE_NEXT_TURN] = {
		CONTEST_EFFECTS_DoubleScoreIn,
		CONTEST_EFFECTS_NextTurn3,
		0,
		18,
		9,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_STEAL_VOLTAGE] = {
		CONTEST_EFFECTS_GetVoltage,
		CONTEST_EFFECTS_FromOneAhead,
		0,
		19,
		7,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_SUPPRESS_VOLTAGE] = {
		CONTEST_EFFECTS_NoVoltageUp,
		CONTEST_EFFECTS_ThisTurn1,
		20,
		20,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
	},
	[CONTEST_EFFECT_RANDOM_ORDER] = {
		CONTEST_EFFECTS_RandomOrder,
		CONTEST_EFFECTS_NextTurn4,
		20,
		21,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_DOUBLE_FINAL_ACT] = {
		CONTEST_EFFECTS_DoubleScore,
		CONTEST_EFFECTS_ForFinalAct,
		20,
		22,
		9,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_LOW_VOLTAGE_ADVANTAGE] = {
		CONTEST_EFFECTS_HighScoreFor1,
		CONTEST_EFFECTS_LowVoltage,
		0,
		23,
		5,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_FIRST_PERFORMANCE_ADVANTAGE] = {
		CONTEST_EFFECTS_IfFirst,
		CONTEST_EFFECTS_PerformancePlusTwo1,
		20,
		24,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_FINAL_PERFORMANCE_ADVANTAGE] = {
		CONTEST_EFFECTS_IfFinal,
		CONTEST_EFFECTS_PerformancePlusTwo2,
		20,
		25,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_NO_VOLTAGE_DOWN] = {
		CONTEST_EFFECTS_NoVoltageDown,
		CONTEST_EFFECTS_ThisTurn2,
		20,
		26,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_TWO_VOLTAGE_IN_A_ROW_ADVANTAGE] = {
		CONTEST_EFFECTS_IfVoltageGoes,
		CONTEST_EFFECTS_UpInARowPlusThree,
		10,
		27,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_HIGH_SCORE_LATER_TURN] = {
		CONTEST_EFFECTS_HighScoreFor2,
		CONTEST_EFFECTS_ALaterTurn,
		0,
		28,
		2,
		29,
		2,
		30,
		2,
		31,
		2,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_MAX_VOLTAGE_ADVANTAGE] = {
		CONTEST_EFFECTS_AfterVoltage,
		CONTEST_EFFECTS_HitsMaxPlusThree,
		20,
		32,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	},
	[CONTEST_EFFECT_PITY_POINTS] = {
		CONTEST_EFFECTS_IfRatedThe,
		CONTEST_EFFECTS_WorstPlusThree,
		10,
		33,
		2,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0,
		0xFFFF,
		0
	}
};


int ContestMove_GetAppealPoints(MoveContestEffect contestEffect) {
	GF_ASSERT(contestEffect < NELEMS(sContestMoves));
	return sContestMoves[contestEffect].appealPoints;
}


int ContestMove_GetDescriptionID(MoveContestEffect contestEffect) {
	GF_ASSERT(contestEffect < NELEMS(sContestMoves));
	return FIRST_CONTEST_EFFECT_DESCRIPTION_ID + (contestEffect - 1);
}
