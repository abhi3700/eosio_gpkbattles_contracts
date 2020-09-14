#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>
#include <vector>
#include <algorithm>
#include <string>

#include "../nlohmann/json.hpp"

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
using eosio::sha256;
using eosio::transaction_size;
using eosio::read_transaction;

using std::string;
using std::vector;

using json = nlohmann::json;

CONTRACT gpkbattlesco : public contract
{
private:
	const symbol gamefee_token_symbol;
	// const symbol gamefee_value;
	const name asset_contract_ac;
	const name escrow_contract_ac;

public:
	using contract::contract;

	gpkbattlesco(name receiver, name code, datastream<const char*> ds) : 
				contract(receiver, code, ds), 
				gamefee_token_symbol("WAX", 4),
				// gamefee_value(asset(50000, symbol("WAX", 4))),		// "5.0000 WAX"
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
	ACTION depositgfee( const name& player,
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
	 * @brief - For WAX RNG Service from Oracle
	 * @details - To receive your random number from the WAX RNG service, your smart contract must include the receiverand
	 * 
	 * @param assoc_id - any no. (not necessarily random), 3700 here.
	 * @param random_value - get transaction from 
	 * 
	 * @return [description]
	 */
	ACTION receiverand(uint64_t assoc_id, const eosio::checksum256& random_value);

	/**
	 * @brief - move the game data
	 * @details - move the info from "ongamestat" to respective players' table in "usergamestat"
	 * 
	 * @param game_id - game id
	 */
	ACTION movegameinfo(uint64_t game_id, const name& player, const string& message);

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
	 * @brief - send alert
	 * @details - send alert after the action is successfully done
	 * 
	 * @param user - user
	 * @param message - note depending on the action
	 */
	ACTION sendalert( const name& user,
						const string& message);


	using empifyplayer_action  = action_wrapper<"empifyplayer"_n, &gpkbattlesco::empifyplayer>;
	using remplayer_action  = action_wrapper<"remplayer"_n, &gpkbattlesco::remplayer>;
	// using empifycards_action  = action_wrapper<"empifycards"_n, &gpkbatescrow::empifycards>;
	// using remcards_action  = action_wrapper<"remcards"_n, &gpkbatescrow::remcards>;

	// -----------------------------------------------------------------------------------------------------------------------
	// check if min. gfeewallet's balance is gamefee_value
	static void check_gfee_balance(const name& player, const asset& game_fee) {
		// instantiate the `gfeewallet` table
		gfeewallet_index gfeewallet_table("gpkbattlesco"_n, player.value);
		auto gfeewallet_it = gfeewallet_table.find(game_fee.symbol.raw());

		check(gfeewallet_it != gfeewallet_table.end(), "the player is not in the wallet table.");
		check(gfeewallet_it->balance.amount >= game_fee.amount, "The player has no min. balance i.e. \'" + 
												game_fee.to_string() + "\' in the game wallet.");
	}

	// -----------------------------------------------------------------------------------------------------------------------
	// check card's category, quality, variant & 2A,1B or 1A,2B before/after the transfer to the contract
	static name checkget_cards_type( const name& asset_contract_ac,
								const name& owner,
								const vector<uint64_t> card_ids,
								const name& category,
								const string& variant ) 
	{
		// create an empty card_ids_type of eosio::name type
		name card_ids_type = ""_n;

		check(card_ids.size() == 3, "the card_ids list chosen must be of size 3");
		vector<string> cardtypes{};

		sassets assets(asset_contract_ac, owner.value);

		for(auto&& card_id : card_ids) {
			auto idx = assets.find(card_id);

			check(idx != assets.end(), "Asset with id " + std::to_string(card_id) + " not found or not yours");
			check (idx->author == "gpk.topps"_n, "Asset is not from this author");
			check(idx->category == category, "The asset id\'s category must be exotic.");

			auto mdata = json::parse(idx->mdata);
			check((mdata["quality"] == "a") || (mdata["quality"] == "b"), "The asset id\'s quality must be either \'a\' or \'b\'."); 
			check(mdata["variant"] == variant, "The asset id\'s variant must be \'base\'.");

			cardtypes.emplace_back(mdata["quality"]);
		}

		// get the respective card types of the given cards
		auto cardtype_1 = cardtypes[0];
		auto cardtype_2 = cardtypes[1];
		auto cardtype_3 = cardtypes[2];

		check(
			// 2A, 1B
			((cardtype_1 == "a") && (cardtype_2 == "a") && (cardtype_3 == "b")) || 
			((cardtype_1 == "a") && (cardtype_2 == "b") && (cardtype_3 == "a")) || 
			((cardtype_1 == "b") && (cardtype_2 == "a") && (cardtype_3 == "a")) ||

			 // 1A, 2B
			((cardtype_1 == "a") && (cardtype_2 == "b") && (cardtype_3 == "b")) || 
			((cardtype_1 == "b") && (cardtype_2 == "a") && (cardtype_3 == "b")) || 
			((cardtype_1 == "b") && (cardtype_2 == "b") && (cardtype_3 == "a")) 
			
			, "the cards chosen are of different combination than (2A,1B) OR (1A,2B). Please, select again."
			);

		if(	// 2A, 1B
			((cardtype_1 == "a") && (cardtype_2 == "a") && (cardtype_3 == "b")) || 
			((cardtype_1 == "a") && (cardtype_2 == "b") && (cardtype_3 == "a")) || 
			((cardtype_1 == "b") && (cardtype_2 == "a") && (cardtype_3 == "a"))
			) {
			card_ids_type = "2a1b"_n;
		}
		else if (	// 1A, 2B
			((cardtype_1 == "a") && (cardtype_2 == "b") && (cardtype_3 == "b")) || 
			((cardtype_1 == "b") && (cardtype_2 == "a") && (cardtype_3 == "b")) || 
			((cardtype_1 == "b") && (cardtype_2 == "b") && (cardtype_3 == "a")) 
			){
			card_ids_type = "1a2b"_n;
		}
	}

	// -----------------------------------------------------------------------------------------------------------------------
	static vector<uint64_t> checkget_3_available_cards(const name& player, const name& asset_contract_ac) {
		// create an empty vector of card
		vector<uint64_t> card_ids{};

		// read the `cardwallet` table & collect 3 available cards of `asset_contract_ac`
		cardwallet_index cardwallet_table("gpkbatescrow"_n, player.value);
		auto usagstatus_idx = cardwallet_table.get_index<"byusagstatus"_n>();
		auto cardwallet_it = usagstatus_idx.find("available"_n.value);

		check( (cardwallet_it != usagstatus_idx.end()) &&
				(cardwallet_it->contract_ac == asset_contract_ac)
				, "player has no cards of asset contract: \'" + asset_contract_ac.to_string() + "\' available for selection.");
		
		// capture the 1st card 
		card_ids.emplace_back(cardwallet_it->card_id);	
		
		// capture the 2 more cards 
		while(card_ids.size() < 3) {
			++cardwallet_it;
			check( (cardwallet_it != usagstatus_idx.end()) && 
				(cardwallet_it->contract_ac == asset_contract_ac)
				, "player has less than 3 available cards. Please ensure min. 3 cards available for selection of asset contract: \'" + asset_contract_ac.to_string() + "\'");
			card_ids.emplace_back(cardwallet_it->card_id);	
		}

		return card_ids;
	}

	// -----------------------------------------------------------------------------------------------------------------------
	static void check_quantity( const asset& quantity ) {
		check(quantity.is_valid(), "invalid quantity");
		check(quantity.amount > 0, "must withdraw positive quantity");
		check(quantity.symbol == symbol("WAX", 4), "symbol precision mismatch");
	}

private:
	// ======================================================================================================================
	// scope - self
	TABLE ongamestat {
		uint64_t game_id;
		name player_1;
		name player_2;
		asset game_fee;
		name asset_contract_ac;
		vector<uint64_t> player1_cards;
		vector<uint64_t> player2_cards;
		name player1_cards_combo;
		name player2_cards_combo;
		uint32_t start_timestamp;			// for draw, start_timestamp & end_timestamp is same.
		uint32_t end_timestamp;				// for no-draw, start timestamp & end_timestamp are different, as there is a wait for RNG service involved of around 1-2 secs.
		name result;						// draw/nodraw
		name winner;
		name loser;
		uint64_t card_won;
		name status;						// /waitdue1draw/over/waitforrng: waitdue1draw(i.e. wait due to 1 draw), over (i.e. game over) & waitforrng (i.e. waiting for rng)
		checksum256 random_value;				// generated from WAX RNG service, if no-draw
		uint8_t draw_count;				// param to monitor the no. of draws
		uint8_t nodraw_count;				// param to ensure that the game is not played again if the count = 1.
		uint8_t total_play_count;			// total play count including - 2 draws, 1 draw >> 1nodraw, 1 nodraw, . Max. is 2. 

		auto primary_key() const { return game_id; }
		uint64_t by_player1() const { return player_1.value; }
		uint64_t by_player2() const { return player_2.value; }
	};

	using ongamestat_index = multi_index<"ongamestat"_n, ongamestat,
								indexed_by< "byplayer1"_n, const_mem_fun<ongamestat, uint64_t, &ongamestat::by_player1>>,	
								indexed_by< "byplayer2"_n, const_mem_fun<ongamestat, uint64_t, &ongamestat::by_player2>>	
								>;
	// -----------------------------------------------------------------------------------------------------------------------
	// scope - player name
	TABLE usergamestat {
		name asset_contract_ac;
		vector<uint64_t> game_ids;
		uint32_t wins;
		uint32_t loses;
		uint32_t draws;
		uint32_t games;
		vector<uint64_t> cards_won;

		auto primary_key() const { return asset_contract_ac.value; }
	};

	using usergamestat_index = multi_index<"usergamestat"_n, usergamestat>;

	// -----------------------------------------------------------------------------------------------------------------------
	/*
		This table gets updated post the game. 
		Basically moved from "ongamestat" to here.
	*/
	// scope - player name
	// TABLE usergamestat {
	// 	uint64_t game_id;				// game_id generated
	// 	vector<uint64_t> played_cards;	// list of played cards
	// 	name played_cards_combo;
	// 	uint32_t start_timestamp;			// for draw, start_timestamp & end_timestamp is same.
	// 	uint32_t end_timestamp;				// for no-draw, start timestamp & end_timestamp are different, as there is a wait for RNG service involved of around 1-2 secs.
	// 	name result;
	// 	name winner;
	// 	name loser;
	// 	uint64_t card_won;
	// 	checksum256 random_value;				// generated from WAX RNG service, if no-draw
	// 	uint8_t draw_count;				// param to monitor the no. of draws
	// 	uint8_t nodraw_count;				// param to ensure that the game is not played again if the count = 1.
	// 	uint8_t total_play_count;			// total play count including - 2 draws, 1 draw >> 1nodraw, 1 nodraw, . Max. is 2. 


	// 	auto primary_key() const { return game_id; }
	// };

	// using usergamestat_index = multi_index<"usergamestat"_n, usergamestat,>;
	// -----------------------------------------------------------------------------------------------------------------------
	// scope - self
	TABLE players {
		name contract_ac;
		vector<name> players_list;

		auto primary_key() const { return contract_ac.value; }
	};

	using players_index = multi_index<"players"_n, players>;
	
	// -----------------------------------------------------------------------------------------------------------------------
	// scope - player name
	TABLE gfeewallet
	{
		asset balance;

		auto primary_key() const { return balance.symbol.raw(); }
	};

	using gfeewallet_index = multi_index<"gfeewallet"_n, gfeewallet>;

	// -----------------------------------------------------------------------------------------------------------------------
	/*
	* Fungible token accounts table which stores information about balances.
	* Scope: token owner
	*/
	struct account {
		uint64_t	id;
		name		author;
		asset		balance;

		uint64_t primary_key()const {
			return id;
		}
	};

	typedef eosio::multi_index< "accounts"_n, account > accounts;


	// scope - owner
	struct sasset {
		uint64_t		id;
		name			owner;
		name			author;
		name			category;
		string			idata;
		string			mdata;
		std::vector<sasset>	container;
		std::vector<account>	containerf;

				
		auto primary_key() const {
			return id;
		}

		uint64_t by_author() const {
			return author.value;
		}
	};

	typedef eosio::multi_index< "sassets"_n, sasset, 		
			eosio::indexed_by< "author"_n, eosio::const_mem_fun<sasset, uint64_t, &sasset::by_author> >
	> sassets;

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
		uint64_t by_usagstatus() const { return usage_status.value; }		
	};

	using cardwallet_index = multi_index<"cardwallet"_n, cardwallet,
							indexed_by< "byusagstatus"_n, eosio::const_mem_fun<cardwallet, uint64_t, &cardwallet::by_usagstatus>>
							>;

	// ==================================================================================
	// get the current timestamp
	inline uint32_t now() const {
		return current_time_point().sec_since_epoch();
	}

	// -----------------------------------------------------------------------------------------------------------------------
	// hex to string
	template<typename CharT>
	static std::string to_hex(const CharT* d, uint32_t s) {
	  std::string r;
	  const char* to_hex="0123456789abcdef";
	  uint8_t* c = (uint8_t*)d;
	  for( uint32_t i = 0; i < s; ++i ) {
	    (r += to_hex[(c[i] >> 4)]) += to_hex[(c[i] & 0x0f)];
	  }
	  return r;
	}

	// -----------------------------------------------------------------------------------------------------------------------
	// get the transaction id
	inline checksum256 get_trxid()
	{
	  auto trxsize = transaction_size();
	  char trxbuf[trxsize];
	  uint32_t trxread = read_transaction( trxbuf, trxsize );
	  check( trxsize == trxread, "read_transaction failed");
	  return sha256(trxbuf, trxsize);
	}

	// -----------------------------------------------------------------------------------------------------------------------
	// get the sha256 hash digest/checksum from (txn_id, timestamp)
	inline checksum256 hash_digest_256(const checksum256& txn_id,
										uint32_t timestamp) const {
		string data_str_cpp = to_hex(&txn_id, sizeof(txn_id)) + std::to_string(timestamp);
		const char * data_str_c = data_str_cpp.c_str(); 

		auto hash_digest = sha256(data_str_c, strlen(data_str_c));

		return hash_digest;
	}


	// -----------------------------------------------------------------------------------------------------------------------
	// convert checksum256 to uint64_t
	inline uint64_t checksum256_to_uint64_t(const checksum256& num) {
		//cast the num to a smaller number
	    uint64_t max_value = 18'446'744'073'709'551'615;		// 2^64 - 1
	    auto byte_array = num.extract_as_byte_array();

	    uint64_t random_int = 0;
	    for (int i = 0; i < 8; i++) {
	        random_int <<= 8;
	        random_int |= (uint64_t)byte_array[i];
	    }

	    uint64_t res = random_int % max_value;

	    return res;
	}

	// -----------------------------------------------------------------------------------------------------------------------
	// find game result in "a" or "b"
	inline name find_game_result(const checksum256& random_val) {
		string s = to_hex(&random_val, sizeof(random_val));

		size_t found_a = s.find("a");
		size_t found_b = s.find("b");

		name res = ""_n;
		if(found_a < found_b) {
			res = "a"_n;
		}
		else if (found_a > found_b) {
			res = "b"_n;
		}

		return res;
	}


	// ==================================================================================
	// Adding inline action for `setgstatus` action in the ridex contract   
	// void set_gstatus( const name& player, 
	// 					uint64_t card_id,
	// 					const name& status );

	// -----------------------------------------------------------------------------------------------------------------------
	// Adding inline action for `sendalert` action in the same contract 
	void send_alert(const name& user, const string& message);
	// -----------------------------------------------------------------------------------------------------------------------
	// Adding inline action for `movegameinfo` action in the same contract 
	void move_game_info(uint64_t game_id, const name& player, const string& message);


};