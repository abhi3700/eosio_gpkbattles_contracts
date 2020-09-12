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
	const name asset_contract_ac;
	const name escrow_contract_ac;

public:
	using contract::contract;

	gpkbattlesco(name receiver, name code, datastream<const char*> ds) : 
				contract(receiver, code, ds), 
				gamefee_token_symbol("WAX", 4),
				asset_contract_ac("simpleassets"_n),
				escrow_contract_ac("gpkbatescrow"_n) {}

	


	/**
	 * @brief - match players & generate game_id
	 * @details - shuffle vector of player list,
	 * 				& take the first 2 elements of the vector
	 * 				
	 * @param asset_contract_ac - asset contract account name
	 */
	ACTION match2player(const name& asset_contract_ac);


	/**
	 * @brief - deposit game fee
	 * @details - deposit game fee
	 * 
	 * @param player - player account name
	 * @param contract_ac - contract account name
	 * @param game_fee - game fee 
	 * @param memo - memo
	 */
	[[eosio::on_notify("eosio.token::transfer")]]
	ACTION deposit( const name& player,
					const name& contract_ac,
					const asset& game_fee,
					const string& memo );


	/**
	 * @brief - player excess withdraw game fee
	 * @details - player excess withdraw game fee
	 * 
	 * @param player - player account name
	 * @param qty - quantity to be withdrawn
	 * 
	 */

	ACTION withdrawgfee( const name& player, 
							const asset& qty);

	/**
	 * @brief - player select cards
	 * @details - player select cards
	 * 			- from FE, show them only transferred cards
	 * 
	 * @param player - player
	 * @param asset_contract_ac - asset contract account name
	 * @param card1_id - card 1 id
	 * @param card2_id - card 2 id
	 * @param card3_id - card 3 id
	 * 
	 * @pre - check the types are 2A1B or 1A2B
	 */
	ACTION sel3card( const name& player,
						const name& asset_contract_ac,
						uint64_t card1_id,
						uint64_t card2_id,
						uint64_t card3_id );

	/**
	 * @brief - contract auto-select cards for player
	 * @details - contract auto-select cards for player, after 30 secs if not selected by player
	 * 
	 * @param player - player
	 * @param asset_contract_ac - asset contract account name
	 *	
	 * @pre - check the types are 2A1B or 1A2B
	 */
	ACTION sel3cardauto( const name& player,
						const name& asset_contract_ac);



	/**
	 * @brief - play the game
	 * @details - play the game with id
	 * 
	 * @param game_id - game id
	 */

	ACTION play(uint64_t game_id);

	/**
	 * @brief - move the game data
	 * @details - move the info from "ongamestat" to respective players' table in "usergamestat"
	 * 
	 * @param game_id - game id
	 */
	ACTION movedb(uint64_t game_id);

	/**
	 * @brief - empify player
	 * @details - empify player
	 * 			- No checks on this, as it has to be used as an inline action here.
	 * 			- If checks are applied & fails then the main action (previous to this) will also fail.
	 * 			- add player. if fails then the transfer asset won't happen due to this.
	 * 
	 * @param asset_contract_ac - simpleassets, atomicassets
	 * @param player - player name
	 * 
	 */
	ACTION empifyplayer(const name& asset_contract_ac, const name& player);

	/**
	 * @brief - remove player
	 * @details - remove player
	 * 			- No checks on this, as it has to be used as an inline action here.
	 * 			- If checks are applied & fails then the main action (previous to this) will also fail.
	 * 			- remove player. if fails then the transfer asset won't happen due to this.
	 * 
	 * @param asset_contract_ac - simpleassets, atomicassets
	 * @param player - player name
	 * 
	 */
	ACTION remplayer(const name& asset_contract_ac, const name& player);

	/**
	 * @brief - empify card
	 * @details - empify card
	 * 			- No checks on this, as it has to be used as an inline action here.
	 * 			- If checks are applied & fails then the main action (previous to this) will also fail.
	 * 			- add card(s). if fails then the transfer asset won't happen due to this.
	 * 
	 * @param asset_contract_ac - simpleassets, atomicassets
	 * @param player - player name
	 * @param cards - 1 or more cards
	 * 
	 */
	ACTION empifycards(const name& asset_contract_ac, const name& player, const vector<uint64_t> cards);


	/**
	 * @brief - remove card
	 * @details - remove card
	 * 			- No checks on this, as it has to be used as an inline action here.
	 * 			- If checks are applied & fails then the main action (previous to this) will also fail.
	 * 			- remove card(s). if fails then the transfer asset won't happen due to this.
	 * 
	 * @param asset_contract_ac - simpleassets, atomicassets
	 * @param player - player name
	 * @param cards - 1 or more cards
	 * 
	 */
	ACTION remcards(const name& asset_contract_ac, const name& player, const vector<uint64_t> cards);


	using empifyplayer_action  = action_wrapper<"empifyplayer"_n, &gpkbatescrow::empifyplayer>;
	using remplayer_action  = action_wrapper<"remplayer"_n, &gpkbatescrow::remplayer>;
	using empifycards_action  = action_wrapper<"empifycards"_n, &gpkbatescrow::empifycards>;
	using remcards_action  = action_wrapper<"remcards"_n, &gpkbatescrow::remcards>;

	// -----------------------------------------------------------------------------------------------------------------------
	static void check_cards_type( const string& cardtype_1,
								const string& cardtype_2,
								const string& cardtype_3 ) 
	{
		check(
			// 2A, 1B
			((cardtype_1 == "a") && (cardtype_2 == "a") && (cardtype_3 == "b")) || 
			((cardtype_1 == "a") && (cardtype_2 == "b") && (cardtype_3 == "a")) || 
			((cardtype_1 == "b") && (cardtype_2 == "a") && (cardtype_3 == "a")) ||

			 // 1A, 2B
			((cardtype_1 == "a") && (cardtype_2 == "b") && (cardtype_3 == "b")) || 
			((cardtype_1 == "b") && (cardtype_2 == "a") && (cardtype_3 == "b")) || 
			((cardtype_1 == "b") && (cardtype_2 == "b") && (cardtype_3 == "a")) || 
			
			, "the cards chosen are of different combination than (2A,1B) OR (1A,2B)."
			);
	}

	static void check_3cards(const name& player, const name& contract_ac) {
		cards_index cards_table(get_self(), player.value);
		auto cards_it = cards_table.find(contract_ac.value);

		check(cards_it != cards_table.end(), "no cards available in the wallet.");
		check(cards_it->cards_list.size() >= 3, "cards_list size is less than 3. Please try to maintain min. 3 cards");
	}

	static void check_quantity( const asset& quantity ) {
		check(quantity.is_valid(), "invalid quantity");
		check(quantity.amount > 0, "must withdraw positive quantity");
		check(quantity.symbol == symbol("WAX", 4), "symbol precision mismatch");
	}

private:
	// -----------------------------------------------------------------------------------------------------------------------
	// scope - self
	TABLE ongamestat {
		uint64_t game_id;
		name player_1;
		name player_2;
		vector<uint64_t> player1_cards;
		vector<uint64_t> player2_cards;
		uint32_t play_timestamp;
		name winner;
		name loser;
		uint64_t card_won;
		// name status;				//	playing

		auto primary_key() const { return game_id; }
		uint64_t by_player1() const { return player_1.value; }
		uint64_t by_player2() const { return player_2.value; }
	};

	using ongamestat_index = multi_index<"ongamestat"_n, ongamestat
								indexed_by< "byplayer1"_n, const_mem_fun<ongamestat, uint64_t, &ongamestat::by_player1> >	
								indexed_by< "byplayer2"_n, const_mem_fun<ongamestat, uint64_t, &ongamestat::by_player2> >	
								>;
	// -----------------------------------------------------------------------------------------------------------------------
	/*
		This table gets updated post the game. 
		Basically moved from "ongamestat" to here.
	*/
	// scope - player name
	TABLE usergamestat {
		uint64_t game_id;				// game_id generated
		vector<uint64_t> played_cards;	// list of played cards
		name win_status;				// y or n

		auto primary_key() const { return game_id; }
	};

	using usergamestat_index = multi_index<"usergamestat"_n, usergamestat,>;
	// -----------------------------------------------------------------------------------------------------------------------
	// scope - self
	TABLE players {
		name contract_ac;
		vector<name> players_list;

		auto primary_key() const { return contract_ac.value; }
	}

	using players_index = multi_index<"players"_n, players>;
	// -----------------------------------------------------------------------------------------------------------------------
	// scope - player name
	TABLE cards {
		name contract_ac;
		vector<uint64_t> cards_list;

		auto primary_key() const { return contract_ac.value; }
	};

	using cards_index = multi_index<"cards"_n, cards>;

	// -----------------------------------------------------------------------------------------------------------------------
	// scope - player name
	TABLE gamewallet
	{
		asset balance;

		auto primary_key() const { return balance.symbol.raw(); }
	};

	using gamewallet_index = multi_index<"gamewallet"_n, gamewallet>;

	// -----------------------------------------------------------------------------------------------------------------------
	// // scope - owner
	// struct sasset {
	// 	uint64_t		id;
	// 	name			owner;
	// 	name			author;
	// 	name			category;
	// 	string			idata;
	// 	string			mdata;
	// 	std::vector<sasset>	container;
	// 	std::vector<account>	containerf;

				
	// 	auto primary_key() const {
	// 		return id;
	// 	}

	// 	uint64_t by_author() const {
	// 		return author.value;
	// 	}
	// };

	// typedef eosio::multi_index< "sassets"_n, sasset, 		
	// 		eosio::indexed_by< "author"_n, eosio::const_mem_fun<sasset, uint64_t, &sasset::by_author> >
	// > sassets;

	// -----------------------------------------------------------------------------------------------------------------------
	// scope - player
	struct cardwallet {
		uint64_t card_id;
		// name quality;		// e.g. a, b
		// string variant;		// e.g. base
		// string category;		// e.g. exotic
		name contract_ac;		// simpleassets, atomicassets
		name usage_status;		// selected/available

		auto primary_key() const { return card_id; }
	};

	using cardwallet_index = multi_index<"cardwallet"_n, cardwallet>;

	// -----------------------------------------------------------------------------------------------------------------------
	// get the current timestamp
	inline uint32_t now() const {
		return current_time_point().sec_since_epoch();
	}

	// Adding inline action for `setgstatus` action in the ridex contract   
	void set_gstatus( const name& player, 
						uint64_t card_id,
						const name& status );


};