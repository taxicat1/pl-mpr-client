#ifndef MPR_STRUCT_POKEMON_H
#define MPR_STRUCT_POKEMON_H

#include "constants/moves.h"
#include "constants/string.h"

#include "struct_defs/seal_case.h"

#include "charcode.h"
#include "mail.h"

/**
 * @brief Block A of the BoxPokemon data structure
 */
typedef struct {
	u16  species;
	u16  heldItem;
	
	u32  otID;
	u32  exp;
	
	u8  friendship;
	u8  ability;
	u8  markings;
	u8  originLanguage;
	
	u8  hpEV;
	u8  atkEV;
	u8  defEV;
	u8  speedEV;
	u8  spAtkEV;
	u8  spDefEV;
	
	u8  cool;
	u8  beauty;
	u8  cute;
	u8  smart;
	u8  tough;
	u8  sheen;
	
	u32 ribbonsDS1; //!< A bitmask of non-Super Contest ribbons introduced in Generation 4
} PokemonDataBlockA;

/**
 * @brief Block B of the BoxPokemon data structure
 */
typedef struct {
	u16  moves[LEARNED_MOVES_MAX];
	u8   moveCurrentPPs[LEARNED_MOVES_MAX];
	u8   movePPUps[LEARNED_MOVES_MAX];
	
	u32  hpIV        : 5;
	u32  atkIV       : 5;
	u32  defIV       : 5;
	u32  speedIV     : 5;
	u32  spAtkIV     : 5;
	u32  spDefIV     : 5;
	u32  isEgg       : 1;
	u32  hasNickname : 1;
	
	u32  ribbonsGBA;  //!< A bitmask of ribbons introduced in Generation 3
	u8   fatefulEncounter : 1;
	u8   gender           : 2;
	u8   form             : 5;
	
	u8   unused1;  //!< First 6 bits track Shiny Leaves from HGSS.
	u16  unused2;
	
	u16  EggLocation_PtHGSS;
	u16  MetLocation_PtHGSS;
} PokemonDataBlockB;

/**
 * @brief Block C of the BoxPokemon data structure
 */
typedef struct {
	charcode_t  nickname[MON_NAME_LEN + 1];  //!< The Pokemon's nickname.
	u8          unused;
	u8          originGame;
	u64         ribbonsDS2;  //!< A bitmask of ribbons given to the Pokemon from Sinnoh's Super Contests.
} PokemonDataBlockC;

/**
 * @brief Block D of the BoxPokemon data structure
 */
typedef struct {
	u16 otName[TRAINER_NAME_LEN + 1];
	
	u8  eggYear;
	u8  eggMonth;
	u8  eggDay;
	
	u8  metYear;
	u8  metMonth;
	u8  metDay;
	
	u16  EggLocation_DP;
	u16  MetLocation_DP;
	
	u8   pokerus;
	u8   pokeball;
	u8   metLevel : 7;
	u8   otGender : 1;
	u8   metTerrain;
	u16  unused;      //!< Stores Apricorn Ball and Mood from HGSS. Unused otherwise.
} PokemonDataBlockD;

typedef union {
	PokemonDataBlockA  blockA;
	PokemonDataBlockB  blockB;
	PokemonDataBlockC  blockC;
	PokemonDataBlockD  blockD;
} PokemonDataBlock;

/**
 * @brief Boxed Pokemon data structure
 */
typedef struct {
	u32  personality;
	u16  partyDecrypted : 1;
	u16  boxDecrypted : 1;
	u16  checksumFailed : 1;
	u16  unused : 13;
	u16  checksum;
	
	PokemonDataBlock dataBlocks[4];
} BoxPokemon;

/**
 * @brief Party Pokemon data structure
 *  This is used to store stats for a pokemon while it is in the players party.
 *  Rather than recalculating stats after each battle, they're stored here.
 */
typedef struct {
	u32  status;         //!< The Pokemon's current status condition.
	u8   level;          //!< The Pokemon's current level, as computed from its total EXP value and its species' level curve.
	u8   ballCapsuleID;  //!< The ID of the ball capsule attached to the Pokemon.
	u16  hp;             //!< The Pokemon's current HP.
	u16  maxHP;          //!< The Pokemon's maximum HP.
	u16  attack;         //!< The Pokemon's Attack stat.
	u16  defense;        //!< The Pokemon's Defense stat.
	u16  speed;          //!< The Pokemon's Speed stat.
	u16  spAtk;          //!< The Pokemon's Special Attack stat.
	u16  spDef;          //!< The Pokemon's Special Defense stat.
	
	Mail         mail;
	BallCapsule  ballCapsule;
} PartyPokemon;

/**
 * @brief The main Pokemon data structure
 */
typedef struct {
	BoxPokemon    box;    //!< Contains the pokemons boxed data
	PartyPokemon  party;  //!< Contains the pokemons extra data while it is in the players party
} Pokemon;

#endif /* MPR_STRUCT_POKEMON_H */
