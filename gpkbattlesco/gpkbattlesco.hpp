#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
#include <vector>
#include <algorithm>
#include <string>

using eosio::contract;
using eosio::print;
using eosio::name;
using eosio::multi_index;
using eosio::const_mem_fun;
using eosio::indexed_by;
using eosio::asset;
using eosio::check;
using eosio::permission_level;
using eosio::datastream;
using eosio::current_time_point;
using eosio::action;
using eosio::same_payer;
using eosio::symbol;
using eosio::require_recipient;
using eosio::checksum256;
using eosio::action_wrapper;

using std::string;
using std::vector;


CONTRACT gpkbattlesco : public contract
{
private:
	const symbol gamefee_token_symbol;

public:
	using contract::contract;

	gpkbattlesco(name receiver, name code, datastream<const char*> ds) : 
				contract(receiver, code, ds), 
				gamefee_token_symbol("WAX", 4) {}

	
	/**
	 * @brief - player select cards
	 * @details - player select cards
	 * 
	 * @param player - player
	 * @param card1_id - card 1 id
	 * @param card2_id - card 2 id
	 * @param card3_id - card 3 id
	 */
	ACTION selectcard( const name& player,
						uint64_t card1_id,
						uint64_t card2_id,
						uint64_t card3_id );

	/**
	 * @brief - match players & generate game_id
	 * @details - randomize 2 vectors of cardtype rows,
	 * 				& take the 1st element of the vector
	 * 
	 */
	ACTION matchplayers();

	/**
	 * @brief - play the game
	 * @details - play the game with id
	 * 
	 * @param game_id - game id
	 */

	ACTION play(uint64_t game_id);

	/**
	 * @brief - move the game data
	 * @details - move the info from "allgameinfo" to respective players' table in "usergameinfo"
	 * 
	 * @param game_id - game id
	 */
	ACTION movedb(uint64_t game_id);


private:
	// -----------------------------------------------------------------------------------------------------------------------
	// scope - self
	TABLE allgameinfo {
		uint64_t game_id;
		name player_1;
		name player_2;
		vector<uint64_t> player1_cards;
		vector<uint64_t> player2_cards;
		uint32_t play_timestamp;
		name winner;
		name loser;
		uint64_t card_won;

		auto primary_key() const { return game_id; }
	};

	using allgameinfo_index = multi_index<"allgameinfo"_n, allgameinfo>;
	// -----------------------------------------------------------------------------------------------------------------------
	/*
		This table gets updated post the game. 
		Basically moved from "allgameinfo" to here.
	*/
	// scope - player name
	TABLE usergameinfo {
		uint64_t game_id;				// game_id generated
		vector<uint64_t> played_cards;	// list of played cards
		name win_status;				// y or n

		auto primary_key() const { return game_id; }
	};

	using usergameinfo_index = multi_index<"usergameinfo"_n, usergameinfo>;
	// -----------------------------------------------------------------------------------------------------------------------
	// scope - self
	TABLE cardtype {
		name type;
		vector<name> players;

		auto primary_key() const { return type.value; }
	}

	using cardtype_index = multi_index<"cardtype"_n, cardtype>;
	// -----------------------------------------------------------------------------------------------------------------------
	// scope - 2a1b, 1a2b
	TABLE selectedcard {
		name player;
		vector<uint64_t> cards;

		auto primary_key() const { return player.value; }
	};

	using selectedcard_index = multi_index<"selectedcard"_n, selectedcard>;

};