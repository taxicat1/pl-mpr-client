#ifndef MPR_POKEMON_H
#define MPR_POKEMON_H

//#include <nitro/rtc.h>

#include "constants/flavors.h"
#include "constants/genders.h"
#include "constants/species_data_params.h"
#include "constants/pokemon_data_params.h"

#include "struct_defs/pokemon.h"
#include "struct_defs/species.h"
#include "struct_defs/sprite_animation_frame.h"

#include "narc.h"
#include "pokemon_anim.h"
#include "pokemon_sprite.h"


#define OTID_NOT_SET    0
#define OTID_SET        1
#define OTID_NOT_SHINY  2

#define FACE_BACK   0
#define FACE_FRONT  2

#define INIT_IVS_RANDOM  32

/**
 * @brief Zeros out a Pokemon data structure, then encrypts the result
 *
 * @param mon
 */
void Pokemon_Init(Pokemon* mon);

/**
 * @brief Zeros out a BoxPokemon data structure, then encrypts the result
 *
 * @param boxMon
 */
void BoxPokemon_Init(BoxPokemon* boxMon);

/**
 * @brief Returns the size in bytes of a Pokemon struct as an int
 *
 * @return Size in bytes of a Pokemon struct
 */
int Pokemon_GetWorkSize(void);

/**
 * @brief Allocates a Pokemon struct on the given heap, then calls ZeroMonData() on it
 *
 * @param heapID
 * @return A new empty but encrypted Pokemon struct
 */
Pokemon* Pokemon_New(HeapID heapID);

/**
 * @brief Decrypts a Pokemon data structure. PartyPokemon data is encrypted using the pokemons personality value, BoxPokemon data using a checksum value
 *
 * @param mon
 * @return Whether the pokemons data was decrypted by this call. Passed to EncryptMon() to decide whether to reencrypt the data or not
 */
BOOL Pokemon_EnterDecryptionContext(Pokemon* mon);

/**
 * @brief Encrypts a Pokemon data structure. PartyPokemon data is encrypted using the pokemons personality value, BoxPokemon data using a checksum value
 *
 * @param mon
 * @param encrypt Whether to encrypt the data or not. If this is false, the function does nothing and returns false.
 * @return Whether the pokemons data was encrypted by this call.
 */
BOOL Pokemon_ExitDecryptionContext(Pokemon* mon, BOOL encrypt);

/**
 * @brief Decrypts a BoxPokemon data structure
 *
 * @param boxMon
 * @return Whether the pokemons data was decrypted by this call. Passed to EncryptBoxMon() to decide whether to reencrypt the data or not
 */
BOOL BoxPokemon_EnterDecryptionContext(BoxPokemon* boxMon);

/**
 * @brief Encrypts a BoxPokemon data structure
 *
 * @param boxMon
 * @param encrypt Whether to encrypt the data or not. If this is false, the function does nothing and returns false.
 * @return Whether the pokemons data was encrypted by this call.
 */
BOOL BoxPokemon_ExitDecryptionContext(BoxPokemon* boxMon, BOOL encrypt);

void Pokemon_InitWith(Pokemon* mon, int monSpecies, int monLevel, int monIVs, BOOL useMonPersonalityParam, u32 monPersonality, int monOTIDSource, u32 monOTID);

/**
 * @brief Calculates and stores the current level and stats for a given Pokemon based on its IVs, EVs etc.
 *
 * @param mon
 */
void Pokemon_CalcLevelAndStats(Pokemon* mon);

/**
 * @brief Calculates and stores the current stats for a given Pokemon based on its IVs, EVs etc.
 *
 * @param mon
 */
void Pokemon_CalcStats(Pokemon* mon);

/**
 * @brief Gets a value from a Pokemon, storing it in dest if neccessary
 *
 * @param mon
 * @param param
 * @param[out] dest Pointer for storing longer data
 * @return The requested value
 */
u32 Pokemon_GetValue(Pokemon* mon, PokemonDataParam param, void* dest);

/**
 * @brief Gets a value from a Pokemon, storing it in dest if neccessary
 *
 * @param boxMon
 * @param param
 * @param[out] dest Pointer for storing longer data
 * @return The requested value
 */
u32 BoxPokemon_GetValue(BoxPokemon* boxMon, PokemonDataParam param, void* dest);

/**
 * @brief Sets a value in a Pokemon, reading it from value if neccessary
 *
 * @param mon
 * @param param
 * @param value
 */
void Pokemon_SetValue(Pokemon* mon, PokemonDataParam param, const void *value);

/**
 * @brief Sets a value in a BoxPokemon, reading it from value if neccessary
 *
 * @param boxMon
 * @param param
 * @param value
 */
void BoxPokemon_SetValue(BoxPokemon* boxMon, PokemonDataParam param, const void *value);

/**
 * @brief Gets a SpeciesData based on a pokemon species
 *
 * @param monSpecies
 * @param heapID The index of the heap that the SpeciesData should be loaded into
 * @return SpeciesData*
 */
SpeciesData* SpeciesData_FromMonSpecies(int monSpecies, HeapID heapID);

/**
 * @brief Gets a value from a SpeciesData structure
 *
 * @param speciesData
 * @param param What value to get
 * @return The requested value
 */
u32 SpeciesData_GetValue(SpeciesData* speciesData, SpeciesDataParam param);

/**
 * @brief Frees a SpeciesData structure from the heap
 *
 * @param speciesData
 */
void SpeciesData_Free(SpeciesData* speciesData);

/**
 * @brief Loads a SpeciesData based on its species and form and gets a value from it
 *
 * @param monSpecies
 * @param monForm
 * @param param What value to get
 * @return The requested value
 */
u32 SpeciesData_GetFormValue(int monSpecies, int monForm, SpeciesDataParam param);

/**
 * @brief Loads a SpeciesData based on its species and gets a value from it
 *
 * @param monSpecies
 * @param param What value to get
 * @return The requested value
 */
u32 SpeciesData_GetSpeciesValue(int monSpecies, SpeciesDataParam param);

/**
 * @brief Gets the amount of expeirence needed for a pokemon species to reach a specified level
 *
 * @param monSpecies
 * @param monLevel
 * @return The amount of exp. needed for the given pokemon species to reach the specified level
 */
u32 Pokemon_GetSpeciesBaseExpAt(int monSpecies, int monLevel);

/**
 * @copybrief GetMonSpeciesLevel()
 *
 * @param mon
 * @return The pokemons level
 */
u32 Pokemon_GetLevel(Pokemon* mon);

/**
 * @copybrief GetMonSpeciesLevel()
 *
 * @param boxMon
 * @return The pokemons level
 */
u32 BoxPokemon_GetLevel(BoxPokemon* boxMon);

/**
 * @brief Gets the level of a pokemon based on its species and exp
 *
 * @param monSpecies
 * @param monExp
 * @return The pokemons level
 */
u32 Pokemon_GetSpeciesLevelAt(u16 monSpecies, u32 monExp);

/**
 * @brief Gets the level of a pokemon based on its personal data and exp
 *
 * @param speciesData
 * @param unused_monSpecies unused
 * @param monExp
 * @return The pokemons level
 */
u32 SpeciesData_GetLevelAt(SpeciesData* speciesData, u16 unused_monSpecies, u32 monExp);

/**
 * @brief Gets the nature of a Pokemon based on its personality value
 *
 * @param mon
 * @return The pokemons nature
 */
u8 Pokemon_GetNature(Pokemon* mon);

/**
 * @brief Gets the nature of a BoxPokemon based on its personality value
 *
 * @param boxMon
 * @return The pokemons nature
 */
u8 BoxPokemon_GetNature(BoxPokemon* boxMon);

/**
 * @brief Gets the nature of a pokemon based on its personality value
 *
 * @param monPersonality
 * @return The pokemons nature
 */
u8 Pokemon_GetNatureOf(u32 monPersonality);

/**
 * @brief Gets the affinitiy of a given pokemon nature to a given stat
 *
 * @param monNature
 * @param statType
 * @return 1 if stat is increased, -1 if stat is decreased, else 0
 */
//s8 Pokemon_GetStatAffinityOf(u8 monNature, u8 statType);

//void Pokemon_UpdateFriendship(Pokemon* mon, u8 friendshipEvent, u16 mapID);

/**
 * @brief Gets the gender of a Pokemon based on its species and personality value
 *
 * @param boxMon
 * @return The pokemons gender
 */
u8 Pokemon_GetGender(Pokemon* mon);

/**
 * @brief Gets the gender of a BoxPokemon based on its species and personality value
 *
 * @param boxMon
 * @return The pokemons gender
 */
u8 BoxPokemon_GetGender(BoxPokemon* boxMon);

/**
 * @brief Gets the gender of a pokemon based on its species and personality value
 *
 * @param monSpecies
 * @param monPersonality
 * @return The pokemons gender
 */
u8 Pokemon_GetGenderOf(u16 monSpecies, u32 monPersonality);

/**
 * @brief Gets the gender of a pokemon based on its SpeciesData and personality value
 *
 * @param speciesData
 * @param unused_monSpecies unused
 * @param monPersonality
 * @return The pokemons gender
 */
u8 SpeciesData_GetGenderOf(SpeciesData* speciesData, u16 unused_monSpecies, u32 monPersonality);

/**
 * @brief Gets whether a BoxPokemon is shiny based on its Original Trainer ID and its personality value
 *
 * @param mon
 * @return Whether the pokemon is shiny or not
 */
u8 Pokemon_IsShiny(Pokemon* mon);

/**
 * @brief Gets whether a pokemon is shiny based on its Original Trainer ID and its personality value
 *
 * @param monOTID
 * @param monPersonality
 * @return Whether the pokemon is shiny or not
 */
u8 Pokemon_IsPersonalityShiny(u32 monOTID, u32 monPersonality);

/**
 * @brief Build a PokemonSpriteTemplate for a Pokemon.
 *
 * @param spriteTemplate Pointer to the sprite template to be populated
 * @param mon            The Pokemon whose data will be used to build the sprite
 * @param face           Which face of the Pokemon the player sees
 */
void Pokemon_BuildSpriteTemplate(PokemonSpriteTemplate* spriteTemplate, Pokemon* mon, u8 face);

/**
 * @brief Build a PokemonSpriteTemplate for a BoxPokemon.
 *
 * @param spriteTemplate Pointer to the sprite template to be populated
 * @param mon            The Pokemon whose data will be used to build the sprite
 * @param face           Which face of the Pokemon the player sees
 */
void BoxPokemon_BuildSpriteTemplate(PokemonSpriteTemplate* spriteTemplate, BoxPokemon* mon, u8 face);

/**
 * @brief Build a PokemonSpriteTemplate for a Pokemon sprite according to the input
 * species, form, and gender.
 *
 * @param spriteTemplate Pointer to the sprite template to be populated
 * @param species        The Pokemon's species
 * @param gender         The Pokemon's gender
 * @param face           Which face of the Pokemon the player sees
 * @param shiny          1 if the Pokemon is shiny, 0 if not
 * @param form           The Pokemon's form
 * @param personality    The Pokemon's personality value
 */
void BuildPokemonSpriteTemplate(PokemonSpriteTemplate* spriteTemplate, u16 species, u8 gender, u8 face, u8 shiny, u8 form, u32 personality);

/**
 * @brief Sanitizes a pokemon form. If the given form is greater than the max for the given species, returns zero, else returns the form unchanged
 *
 * @param monSpecies
 * @param monForm
 * @return The sanitized pokemon form
 */
u8 Pokemon_SanitizeFormId(u16 monSpecies, u8 monForm);

void Pokemon_LoadSpriteAnimationFrames(SpriteAnimFramePt* animFrames, u16 species);
void Pokemon_LoadSpriteAnimation(PokemonAnimManager* monAnimMan, PokemonSprite* monSprite, u16 species, int face, int nature, BOOL flipSprite, int index);

/**
 * @brief Returns the size in bytes of a Pokemon struct as a u32
 *
 * @return Size in bytes of a Pokemon struct
 */
u32 Pokemon_GetStructSize(void);

/**
 * @brief Returns the size in bytes of a BoxPokemon struct as a u32
 *
 * @return Size in bytes of a BoxPokemon struct
 */
u32 BoxPokemon_GetStructSize(void);

/**
 * @brief Gets the form of a Pokemon
 *
 * @param mon
 * @return The pokemons form
 */
u8 Pokemon_GetForm(Pokemon* mon);

/**
 * @brief Gets the form of a BoxPokemon
 *
 * @param boxMon
 * @return The pokemons form
 */
u8 BoxPokemon_GetForm(BoxPokemon* boxMon);

/**
 * @brief Gets the BoxPokemon data for a given Pokemon
 *
 * @param mon
 * @return The BoxPokemon data for a given Pokemon
 */
BoxPokemon* Pokemon_GetBoxPokemon(Pokemon* mon);

/**
 * @brief Adds a move to the moveset of a Pokemon
 *
 * @param mon
 * @param moveID
 * @return The given moveID if successful, 0xfffe if already known, 0xffff if there is no room for the move
 */
u16 Pokemon_AddMove(Pokemon* mon, u16 moveID);

/**
 * @brief Deletes the first move of a Pokemon and adds the given move to the end of its moveset
 *
 * @param mon
 * @param moveID
 */
void Pokemon_ReplaceMove(Pokemon* mon, u16 moveID);

/**
 * @brief Sets the given moveSlot of a Pokemon, removing its PP Ups
 *
 * @param mon
 * @param moveID
 * @param moveSlot
 */
void Pokemon_ResetMoveSlot(Pokemon* mon, u16 moveID, u8 moveSlot);

/**
 * @brief Sets the given moveSlot of a Pokemon, retaining the PP Ups for that slot
 *
 * @param mon
 * @param moveID
 * @param moveSlot
 */
void Pokemon_SetMoveSlot(Pokemon* mon, u16 moveID, u8 moveSlot);

//u16 Pokemon_LevelUpMove(Pokemon* mon, int *index, u16 *moveID);

/**
 * @brief Swaps the places of two moves on a Pokemon
 *
 * @param mon
 * @param moveSlot1
 * @param moveSlot2
 */
void Pokemon_SwapMoveSlots(Pokemon* mon, int moveSlot1, int moveSlot2);

/**
 * @brief Swaps the places of two moves on a BoxPokemon
 *
 * @param boxMon
 * @param moveSlot1
 * @param moveSlot2
 */
void BoxPokemon_SwapMoveSlots(BoxPokemon* boxMon, int moveSlot1, int moveSlot2);

/**
 * @brief Deletes the given moveSlot of a Pokemon, shifting the ones above it down
 *
 * @param mon
 * @param moveSlot
 */
void Pokemon_ClearMoveSlot(Pokemon* mon, u32 moveSlot);

void Pokemon_FromBoxPokemon(BoxPokemon* boxMon, Pokemon* mon);

/**
 * @brief Gets the Sinnoh Pokedex number of a pokemon from its National Pokedex number
 *
 * @param species The National Pokedex number of a pokemon
 * @return The Sinnoh Pokedex number of that pokemon (or zero if none exists)
 */
u16 Pokemon_SinnohDexNumber(u16 species);

/**
 * @brief Gets the affinitiy of a given Pokemon to a given flavor
 *
 * @param mon
 * @param flavor
 * @return 1 if liked flavor, -1 if disliked flavor, else 0
 */
s8 Pokemon_GetFlavorAffinity(Pokemon* mon, Flavor flavor);

/**
 * @brief Gets the affinitiy of a given Pokemon personality to a given flavor
 *
 * @param monPersonality
 * @param flavor
 * @return 1 if liked flavor, -1 if disliked flavor, else 0
 */
s8 Pokemon_GetFlavorAffinityOf(u32 monPersonality, Flavor flavor);

BOOL Pokemon_IsInfectedWithPokerus(Pokemon* mon);
BOOL Pokemon_HasCuredPokerus(Pokemon* mon);

/**
 * @brief Sets Arceus' form based on its held item. Has no effect if the given Pokemon is not an Arceus
 *
 * @param boxMon
 */
void BoxPokemon_SetArceusForm(BoxPokemon* boxMon);

/**
 * @brief Get Arceus' form given an items hold effect
 *
 * @param itemHoldEffect
 * @return The form arceus should be in
 */
u8 Pokemon_GetArceusTypeOf(u16 itemHoldEffect);

/**
 * @brief Sets Rotom to the given form. Has no effect if the given Pokemon is not a Rotom
 * If Rotom should learn a form specific move and there is no room, overwrites moveSlot with the new move
 *
 * @param mon
 * @param form
 * @param moveSlot
 * @return Whether the given pokemon was a Rotom
 */
BOOL Pokemon_SetRotomForm(Pokemon* mon, int form, int moveSlot);

/**
 * @brief Loads a Level-Up move table based on a pokemon species and form into the pointed to array
 *
 * @param monSpecies
 * @param monForm
 * @param[out] monLevelUpMoves Pointer to a u16 array to store the move table
 */
void Pokemon_LoadLevelUpMovesOf(int monSpecies, int monForm, u16* monLevelUpMoves);

/**
 * @brief Sets the ability of a Pokemon based on its species, form and peronsality value
 *
 * @param mon
 */
void Pokemon_CalcAbility(Pokemon* mon);


void BoxPokemon_RestorePP(BoxPokemon* boxMon);

/**
 * @brief Load the animation frames for a given species and a client type (Platinum only)
 * (implicitly defining which face of the sprite is visible to the player).
 *
 * @param[out] frames   Out-param for the loaded frame data
 * @param species       Species to be loaded
 * @param clientType    Client-type of who made the load request
 */
void PokemonSprite_LoadAnimFramesPt(SpriteAnimFramePt* frames, u16 species, u16 clientType);

/**
 * @brief Load the animation data for a given species and a client type (Platinum only)
 *
 * @param monAnimMan    Pokemon animation manager
 * @param sprite        Pre-loaded Pokemon sprite
 * @param species       Species to be loaded
 * @param face          Which face is visible to the player
 * @param flipSprite    If TRUE, flip the sprite + animation
 * @param frame         Which frame of the animation to initialize
 */
void PokemonSprite_LoadAnimPt(PokemonAnimManager* monAnimMan, PokemonSprite* sprite, u16 species, int face, int flipSprite, int frame);

#endif /* MPR_POKEMON_H */
