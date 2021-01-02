#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
#include <vector>
#include <algorithm>
#include <string>

#include "../../libs/nlohmann/json.hpp"

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

using json = nlohmann::json;


CONTRACT gpkbatescrow : public contract
{
private:
	const int64_t gamefee_token_amount;
	const symbol gamefee_token_symbol;
	const name game_contract_ac;
	const name card_author;

public:
	using contract::contract;

	gpkbatescrow(name receiver, name code, datastream<const char*> ds) : 
				contract(receiver, code, ds), 
				gamefee_token_amount(500000000),
				gamefee_token_symbol("WAX", 8),
				game_contract_ac("gpkbattlesco"_n),				// For Mainnet
				// game_contract_ac("gpkbattlesc1"_n),					// For Testnet
				card_author("gpk.topps"_n)							// For Mainnet
				// card_author("gpkbattlesco"_n)						// For Testnet
				{}

	/**
	 * @brief - transfer assets by player
	 * @details - transfer assets by player. Any no. of cards can be transferred at once to save the CPU, NET resources of player.
	 * 
	 * @param player - player who transfers
	 * @param asset_contract_ac - simpleassets, atomicassets
	 * @param card_ids - 1 or more cards
	 * @param memo - a note
	 * 
	 * NOTE: No check of "2a1b" or "1a2b" card combo as any no. of cards can be transferred at once to save the CPU, NET resources
	 */
	
	[[eosio::on_notify("*::transfer")]]
	void transferbypl( const name& player, 
							const name& to, 
							const vector<uint64_t>& card_ids, 
							const string& memo );



	/**
	 * @brief - an inline action for battle contract
	 * @details - battle contract calls this action externally
	 * 
	 * @param player - whose card's status will be transferred
	 * @param card_id - card id
	 * @param status - available/selected
	 * 
	 * @pre - status should be either "available" or "selected"
	 * @pre - card should have been transferred by the player
	 */
	ACTION setcstatus( const name& player, 
						uint64_t card_id,
						const name& status );

	/**
	 * @brief - player withdraws the card, if not selected for game i.e. "available" status
	 * @details - player withdraws the card, if not selected for game i.e. "available" status
	 * 
	 * @param player - player who withdraws
	 * @param asset_contract_ac - simpleassets, atomicassets
	 * @param card_id - card id
	 * 
	 * @pre - the card_id should not be "selected" status
	 */
	ACTION withdrawbypl( const name& player,
							const name& asset_contract_ac,
							uint64_t card_id );

	/**
	 * @brief - an inline action for battle contract usage externally
	 * @details - disburses winner cards (4 qty.) to winner
	 * 			- disburses loser cards (2 qty.) to loser
	 * 			- alerts the user with win/lose status with game_id	 
	 * 
	 * @param game_id - for adding into memo note
	 * 
	 * @pre - Check that the game_id's status is marked "over" before disbursement of the cards
	 * @pre - Check that the game_id's result is marked "nodraw" before disbursement of the cards
	 * @pre - winner_transfer_cards size must be 4
	 * @pre - loser_transfer_cards size must be 2
	 */
	ACTION disburse( uint64_t game_id );

	ACTION testsetcstat( const name& player, uint64_t card_id, const name& status ) {
		// instantiate the `cardwallet` table
		cardwallet_index cardwallet_table(get_self(), player.value);
		auto card_it = cardwallet_table.find(card_id);

		check(card_it != cardwallet_table.end(), "card with id:" + std::to_string(card_id) + " doesn't exist in the table.");

		cardwallet_table.modify(card_it, get_self(), [&](auto& row) {
			row.usage_status = status;
		});
	}

	using setcstatus_action  = action_wrapper<"setcstatus"_n, &gpkbatescrow::setcstatus>;
	using disburse_action  = action_wrapper<"disburse"_n, &gpkbatescrow::disburse>;

	// -----------------------------------------------------------------------------------------------------------------------
	// check card's category, quality, variant & 2A,1B or 1A,2B before/after the transfer to the contract
	static void check_cards_type( const name& asset_contract_ac,
								const name& owner,
								const vector<uint64_t> card_ids,
								const name& category,
								const string& variant ) 
	{
		// check(card_ids.size() == 3, "the card_ids list chosen must be of size 3");
		// vector<string> cardtypes{};

		sassets assets(asset_contract_ac, owner.value);

		for(auto&& card_id : card_ids) {
			auto idx = assets.find(card_id);

			check(idx != assets.end(), "Asset with id " + std::to_string(card_id) + " not found or not yours");
			check (idx->author == "gpk.topps"_n, "Asset is not from this author");				// for WAX Mainnet
			// check (idx->author == "gpkbattlesco"_n, "Asset is not from this author");				// for WAX Testnet
			check(idx->category == category, "The asset id\'s category must be exotic.");

			auto mdata = json::parse(idx->mdata);
			check((mdata["quality"] == "a") || (mdata["quality"] == "b"), "The asset id\'s quality must be either \'a\' or \'b\'."); 
			check(mdata["variant"] == variant, "The asset id\'s variant must be \'base\'.");

			// cardtypes.emplace_back(mdata["quality"]);
		}
/*
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
			
			, "the cards chosen are of different combination than (2A,1B) OR (1A,2B)."
			);
*/	}

	// -----------------------------------------------------------------------------------------------------------------------
	// check if min. gfeewallet's balance is gamefee_value
	static void check_gfee_balance(const name& player, const asset& game_fee) {
		// instantiate the `gfeewallet` table
		gfeewallet_index gfeewallet_table("gpkbattlesco"_n, player.value);				// for WAX Mainnet
		// gfeewallet_index gfeewallet_table("gpkbattlesc1"_n, player.value);				// for WAX Testnet
		auto gfeewallet_it = gfeewallet_table.find(game_fee.symbol.raw());

		check(gfeewallet_it != gfeewallet_table.end(), "the player is not in the gamefee wallet table.");
		check(gfeewallet_it->balance.amount >= game_fee.amount, "The player has no min. balance i.e. \'" + 
												game_fee.to_string() + "\' in the gamefee wallet.");
	}

private:
	// -----------------------------------------------------------------------------------------------------------------------
	// scope - player
	TABLE cardwallet {
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
							eosio::indexed_by< "byusagstatus"_n, eosio::const_mem_fun<cardwallet, uint64_t, &cardwallet::by_usagstatus> >
	>;

	// -----------------------------------------------------------------------------------------------------------------------
	// scope - self
	struct ongamestat {
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
		vector<uint64_t> winner_transfer_cards;		// of size 4 after the game is nodraw
		vector<uint64_t> loser_transfer_cards;		// of size 2 after the game is nodraw
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
	struct gfeewallet
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


};