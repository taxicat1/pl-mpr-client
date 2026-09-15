#ifndef MPR_MPR_CLIENT_MPR_TEXT_H
#define MPR_MPR_CLIENT_MPR_TEXT_H

#include "charcode.h"
#include "string_gf.h"

const charcode_t* MPRText_GetChars(int messageID);
String* MPRText_CopyChars(String* dest, int messageID);

#define MPR_TEXT_FLAG  0x8000

#define MPR_TEXT_Yes 0
#define MPR_TEXT_No 1
#define MPR_TEXT_DPPtOnly 2
#define MPR_TEXT_SaveCorrupt 3
#define MPR_TEXT_RestartAndSave 4
#define MPR_TEXT_WarningSplashHeader 5
#define MPR_TEXT_WarningSplashBody 6
#define MPR_TEXT_WarningSaveBody 7
#define MPR_TEXT_PressA 8
#define MPR_TEXT_ConnectedToRanch 9
#define MPR_TEXT_DepositToRanch 10
#define MPR_TEXT_WithdrawFromRanch 11
#define MPR_TEXT_Save 12
#define MPR_TEXT_Quit 13
#define MPR_TEXT_Connecting 14
#define MPR_TEXT_FailedToConnect 15
#define MPR_TEXT_Disconnected 16
#define MPR_TEXT_TurningOffPower 17
#define MPR_TEXT_WaitingForWii 18
#define MPR_TEXT_SaveAndQuitConfirm 19
#define MPR_TEXT_QuitConfirm 20
#define MPR_TEXT_QuitWithoutSavingConfirm 21
#define MPR_TEXT_Saving 22
#define MPR_TEXT_SaveOK 23
#define MPR_TEXT_SaveFailed 24
#define MPR_TEXT_SortPokemon 25
#define MPR_TEXT_RefineSearch 26
#define MPR_TEXT_DepositedMon 27
#define MPR_TEXT_PokedexNo 28
#define MPR_TEXT_Name 29
#define MPR_TEXT_Nickname 30
#define MPR_TEXT_Level 31
#define MPR_TEXT_Item 32
#define MPR_TEXT_HP 33
#define MPR_TEXT_Attack 34
#define MPR_TEXT_Defense 35
#define MPR_TEXT_SpAttack 36
#define MPR_TEXT_SpDefense 37
#define MPR_TEXT_Speed 38
#define MPR_TEXT_Weight 39
#define MPR_TEXT_Height 40
#define MPR_TEXT_Country 41
#define MPR_TEXT_Random 42
#define MPR_TEXT_Reset 43
#define MPR_TEXT_OK 44
#define MPR_TEXT_SortAlpha1 45
#define MPR_TEXT_SortAlpha2 46
#define MPR_TEXT_SortAlpha3 47
#define MPR_TEXT_SortAlpha4 48
#define MPR_TEXT_SortAlpha5 49
#define MPR_TEXT_SortAlpha6 50
#define MPR_TEXT_SortAlpha7 51
#define MPR_TEXT_SortAlpha8 52
#define MPR_TEXT_SortAlpha9 53
#define MPR_TEXT_SortAlpha10 54
#define MPR_TEXT_Type 55
#define MPR_TEXT_Move 56
#define MPR_TEXT_Ability 57
#define MPR_TEXT_Nature 58
#define MPR_TEXT_Mark 59
#define MPR_TEXT_SortBasic 60
#define MPR_TEXT_SortStats 61
#define MPR_TEXT_SortOther 62
#define MPR_TEXT_Colon 63
#define MPR_TEXT_Dash 64
#define MPR_TEXT_PCDeposit 65
#define MPR_TEXT_PCWithdraw 66
#define MPR_TEXT_PCRight10 67
#define MPR_TEXT_PCLeft10 68
#define MPR_TEXT_PCFirst 69
#define MPR_TEXT_PCLast 70
#define MPR_TEXT_PCDepositQuitConfirm 71
#define MPR_TEXT_PCWithdrawQuitConfirm 72
#define MPR_TEXT_PCDepositContinueConfirm 73
#define MPR_TEXT_PCWithdrawContinueConfirm 74
#define MPR_TEXT_DepositMonConfirm 75
#define MPR_TEXT_MonDeposited 76
#define MPR_TEXT_MonHaveAGoodTime 77
#define MPR_TEXT_RanchName 78
#define MPR_TEXT_UNUSED_0079 79
#define MPR_TEXT_WithdrawnMon 80
#define MPR_TEXT_MonWelcomeBack 81
#define MPR_TEXT_AllDSBoxesFull 82
#define MPR_TEXT_RanchFull 83
#define MPR_TEXT_ItsEmpty 84
#define MPR_TEXT_RanchNotEnoughSpace 85
#define MPR_TEXT_DepositAsManyPokemonConfirm 86
#define MPR_TEXT_DepositAllPokemonConfirm 87
#define MPR_TEXT_DepositedAll 88
#define MPR_TEXT_DepositedNumMons 89
#define MPR_TEXT_CannotDeposit 90
#define MPR_TEXT_ItsEmpty2 91
#define MPR_TEXT_BoxNotEnoughSpace 92
#define MPR_TEXT_WithdrawAsManyPokemonConfirm 93
#define MPR_TEXT_WithdrawnAll 94
#define MPR_TEXT_WithdrawnNumMons 95
#define MPR_TEXT_WithdrawWhichBox 96
#define MPR_TEXT_BoxNotEnoughSpace2 97
#define MPR_TEXT_BoxFull 98
#define MPR_TEXT_WhatDoYouWant 99
#define MPR_TEXT_JumpWhichBox 100
#define MPR_TEXT_RotomReverted 101
#define MPR_TEXT_ErrorOccurred 102

#define MPR_TEXT_COUNT 103

#endif /* MPR_MPR_CLIENT_MPR_TEXT_H */
