#ifndef MPR_GLOBAL_TRADE_H
#define MPR_GLOBAL_TRADE_H

#include "pokemon.h"
#include "savedata.h"

typedef struct {
	u8   storedPokemon[sizeof(Pokemon)];
	u16  pokemonListed;
	u16  unused_EE;
	u32  tradeDatestamp;
} GlobalTradeDP;

typedef struct {
	u8   storedPokemon[sizeof(Pokemon)];
	u16  pokemonListed;
	u16  unused_EE;
	u32  depositTradeDatestamp;
	u32  searchTradeDatestamp;
} GlobalTradePt;

typedef struct {
	u8   storedPokemon[sizeof(Pokemon)];
	u16  pokemonListed;
	u16  unused_EE;
} GlobalTradeCommon;

int GlobalTrade_GetSaveSize(void);
void GlobalTrade_Init(GlobalTradeCommon* globalTrade);

#endif /* MPR_GLOBAL_TRADE_H */
