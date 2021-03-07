#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>
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
using eosio::sha256;
using eosio::transaction_size;
using eosio::read_transaction;

using std::string;
using std::vector;
using std::pair;

using json = nlohmann::json;

CONTRACT gpkbattlesco : public contract
{
private:
	const int64_t gamefee_token_amount;
	const symbol gamefee_token_symbol;
	// const asset gamefee_value;
	// const name asset_contract_ac;
	const name escrow_contract_ac;
	const name income_contract_ac;
	name paired_player2;
	uint8_t paired_player2_count;

	// 370015336 for "simpleassets", 370015337 for "atomicassets"
	// const vector<pair<name, uint64_t>> vector_assetcontracts_associds;						// a list of pair of asset contract account names & assoc_ids

	// name card_ids_type_1, card_ids_type_2;

public:
	using contract::contract;

	gpkbattlesco(name receiver, name code, datastream<const char*> ds) : 
				contract(receiver, code, ds), 
				gamefee_token_amount(500000000),
				gamefee_token_symbol("WAX", 8),
				// gamefee_value(asset(50000, symbol("WAX", 8))),		// "5.00000000 WAX"
				// asset_contract_ac("simpleassets"_n),
				escrow_contract_ac("gpkbatescrow"_n),
				income_contract_ac("gpkbatincome"_n),
				paired_player2(""_n),
				paired_player2_count(0)
				// vector_assetcontracts_associds({{"simpleassets"_n, 370015336}, {"atomicassets"_n, 370015337}}),
				// card_ids_type_1("2a1b"_n),
				// card_ids_type_2("1a2b"_n)
				{}

	/**
	 * @brief set config of cards with corresponding game_fee
	 * @details set config of cards with corresponding game_fee
	 * 
	 * @param asset_contract_ac - asset contract account name: simpleassets, atomicassets
	 * @param id - card's combo id
	 * @param author - card's author
	 * @param category - card's category
	 * @param variant - card's variant
	 * @param quality - card's author
	 * @param game_fee - card's game_fee
	 */
	ACTION setconfig( const name& asset_contract_ac,
						uint64_t id,
						const name& author, 
						const name& category,
						const string& variant,
						const string& quality,
						const asset& game_fee );

	// read config table 1: via id
	ACTION testrdconft1( const name& asset_contract_ac, uint64_t id ) {
		require_auth(get_self());

		config_index config_table(get_self(), asset_contract_ac.value);
		auto config_it = config_table.find(id);

		check(config_it != config_table.end(), "the combo id is not found.");
		
		print(std::to_string(id));
		// print(std::to_string(id), config_it->author.to_string(), config_it->category.to_string(), config_it->variant, config_it->quality, config_it->game_fee.to_string());
		// config_it->game_fee.print();

	}

	// read config table 2: via author
	ACTION testrdconft2( const name& asset_contract_ac, const name& author ) {
		require_auth(get_self());

		config_index config_table(get_self(), asset_contract_ac.value);
		auto author_idx = config_table.get_index<"byauthor"_n>();
		auto config_it = author_idx.find(author.value);

		check(config_it != author_idx.end(), "the combo id is not found.");

		while(config_it != author_idx.end()) {
			print(std::to_string(config_it->id), config_it->author.to_string(), config_it->category.to_string(), config_it->variant, config_it->quality, config_it->game_fee.to_string());
			// config_it->game_fee.print();
			++config_it;
		}
	}


	/**
	 * @brief del config of cards with combo id
	 * @details del config of cards with combo id
	 * 
	 * @param asset_contract_ac - asset contract account name: simpleassets, atomicassets
	 * @param id - card's combo id
	 */
	ACTION delconfig( const name& asset_contract_ac, 
						uint64_t id );


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
	void depositgfee( const name& player,
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
	 * @brief - add game fee to player account by qty
	 * @details - add game fee to player account by qty
	 * 			- mainly used during play, del1drawgame ACTION
	 * 
	 * @param player - player
	 * @param qty - quantity
	 * 
	 */
	ACTION inliincplbal( const name& player,
							const asset& qty );

	/**
	 * @brief - deduct game fee to player account by qty
	 * @details - deduct game fee to player account by qty
	 * 			- mainly used during play, del1drawgame ACTION
	 * 
	 * @param player - player
	 * @param qty - quantity
	 * 
	 */
	ACTION inlidecplbal( const name& player,
							const asset& qty );

	/**
	 * @brief - transfer game fee to income account
	 * @details - transfer game fee to income account
	 * 
	 * @param player - player
	 * @param qty - quantity
	 * 
	 */
/*	ACTION trincomegfee( const name& player, 
							const asset& qty );
*/
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
/*	ACTION sel3cardauto( const name& player,
						const name& asset_contract_ac);
*/


	/**
	 * @brief - match players & generate game_id
	 * @details - shuffle vector of player list,
	 * 				& take the first 2 elements of the vector
	 * 
	 * @param player_1 - player who presses the pair button				
	 * @param asset_contract_ac - asset contract account name
	 */
	ACTION pairwplayer(const name& player_1, 
						const name& asset_contract_ac);

	/**
	 * @brief - play the game
	 * @details - play the game with id
	 * 
	 * @param game_id - game id
	 */
	ACTION play(uint64_t game_id);

	/**
	 * @brief - play the game if both the player doesn't select cards within 180s of time.
	 * @details - activities:
	 * 				- return the deducted money during 1-draw to the non-defaulter
	 * 				- delete the game_id from ongamestat table
	 * 
	 * @param game_id - game id
	 * @param defaulter_pl - defaulter player
	 */
	ACTION del1drawgame( uint64_t game_id,
						const vector<name>& defaulter_pl_list );

	/**
	 * @brief - For WAX RNG Service from Oracle
	 * @details - To receive your random number from the WAX RNG service, your smart contract must include the receiverand
	 * 
	 * @param assoc_id - any no. (not necessarily random), 3700 here.
	 * @param random_value - get transaction from 
	 */
	ACTION receiverand(uint64_t assoc_id, const eosio::checksum256& random_value);

	/**
	 * @brief - disburse nodraw cards after game is over
	 * @details - disburse nodraw cards after game is over
	 * 
	 * @param game_id - game_id
	 */
	ACTION disndcards( uint64_t game_id );

	/**
	 * @brief - move & erase (i.e. moer) the game data for both the players after game in 3 cases: "1 Draw, 1 NoDraw", "2 times Draw" & "NoDraw" 
	 * @details - move the info from "ongamestat" to respective players' table in "usergamestat"
	 * 			- Note that in case of 1 time Draw, the players are given one last chance to select cards. So, no movement of data. 
	 * @param game_id - game id
	 */
	ACTION moergameinfo(uint64_t game_id, const string& message);

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


	// ACTION sendmsgplyrs(uint64_t game_id, const string& msg);


	/**
	 * @brief - send alert
	 * @details - send alert after the action is successfully done
	 * 
	 * @param user - user
	 * @param message - note depending on the action
	 */
	ACTION sendalert( const name& user,
						const string& message);



	// for testing the escrow table by reading the cards for `select3cardauto` ACTION
/*	ACTION testrdescrow( const name& player, const name& asset_contract_ac ) {
		require_auth(get_self());
		auto card_ids = checkget_3_available_cards(player, asset_contract_ac);

		for(auto&& card_id : card_ids) {
			send_alert(player, std::to_string(card_id));			// to check if the cards are captured properly	
			// print(std::to_string(card_id), " | ");
		}
	}
*/
	ACTION testdelongam( uint64_t game_id ) {
		require_auth(get_self());
		ongamestat_index ongamestat_table(get_self(), get_self().value);
		auto ongamestat_it = ongamestat_table.find(game_id);
		check(ongamestat_it != ongamestat_table.end(), "The game_id doesn't exist." );


		// mark the cards as "available" for both players in `cardwallet` table of escrow contract
		// modify card's status as "available" in `cardwallet` table of escrow contract
		// For player_1
		for(auto&& card_id : ongamestat_it->player1_cards) {
			action(
				permission_level{get_self(), "active"_n},
				escrow_contract_ac,
				"setcstatus"_n,
				std::make_tuple(ongamestat_it->player_1, card_id, "available"_n)
			).send();
		}
		// For player_2
		for(auto&& card_id : ongamestat_it->player2_cards) {
			action(
				permission_level{get_self(), "active"_n},
				escrow_contract_ac,
				"setcstatus"_n,
				std::make_tuple(ongamestat_it->player_2, card_id, "available"_n)
			).send();
		}

		ongamestat_table.erase(ongamestat_it);
	}

	ACTION testaddplayr(const name& asset_contract_ac, const name& player) {
		require_auth(get_self());
		// add player to the players_list, if not added
		players_index players_table(get_self(), get_self().value);
		auto players_it = players_table.find(asset_contract_ac.value);

		// check(players_it == players_table.end(), "there is players list with this asset contract ac");

		if(players_it == players_table.end()) {
			players_table.emplace(get_self(), [&](auto& row){
				row.asset_contract_ac = asset_contract_ac;
				row.players_list = vector<name>{player};
			});
		} else {
			auto vec_it = std::find(players_it->players_list.begin(), players_it->players_list.end(), player);
			if(vec_it == players_it->players_list.end()) {
				players_table.modify(players_it, get_self(), [&](auto& row) {
					row.players_list.emplace_back(player);
				});
			}
		}
	}


	ACTION testremplayr(const name& asset_contract_ac, const name& player) {
		require_auth(get_self());
		// remove player to the players_list, if present
		players_index players_table(get_self(), get_self().value);
		auto players_it = players_table.find(asset_contract_ac.value);

		check(players_it != players_table.end(), "there is no players list with this asset contract ac");

		auto vec_it = std::find(players_it->players_list.begin(), players_it->players_list.end(), player);
		if(vec_it != players_it->players_list.end()) {
			players_table.modify(players_it, get_self(), [&](auto& row) {
				row.players_list.erase(vec_it);
			});
		}
	}

	ACTION testdelugame(const name& player, const name& asset_contract_ac) {
		require_auth(get_self());
		usergamestat_index usergamestat_table(get_self(), player.value);
		auto usergamestat_player_it = usergamestat_table.find(asset_contract_ac.value);

		check(usergamestat_player_it != usergamestat_table.end(), "there is no row created for this asset type.");

		usergamestat_table.erase(usergamestat_player_it);
	}


	using empifyplayer_action  = action_wrapper<"empifyplayer"_n, &gpkbattlesco::empifyplayer>;
	using remplayer_action  = action_wrapper<"remplayer"_n, &gpkbattlesco::remplayer>;
	// using empifycards_action  = action_wrapper<"empifycards"_n, &gpkbatescrow::empifycards>;
	// using remcards_action  = action_wrapper<"remcards"_n, &gpkbatescrow::remcards>;

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

	// -----------------------------------------------------------------------------------------------------------------------
	// static vector<uint64_t> checkget_3_available_cards(const name& player, const name& asset_contract_ac) {
	// 	// create an empty vector of card
	// 	vector<uint64_t> card_ids{};

	// 	// read the `cardwallet` table & collect 3 available cards of `asset_contract_ac`
	// 	cardwallet_index cardwallet_table("gpkbatescrow"_n, player.value);
	// 	auto usagstatus_idx = cardwallet_table.get_index<"byusagstatus"_n>();
	// 	auto cardwallet_it = usagstatus_idx.find("available"_n.value);

	// 	check( (cardwallet_it != usagstatus_idx.end()) &&
	// 			(cardwallet_it->contract_ac == asset_contract_ac)
	// 			, "player has no cards of asset contract: \'" + asset_contract_ac.to_string() + "\' available for selection.");
		
	// 	// capture the 1st card 
	// 	card_ids.emplace_back(cardwallet_it->card_id);	
		
	// 	// capture the 2 more cards 
	// 	while(card_ids.size() < 3) {
	// 		++cardwallet_it;
	// 		check( (cardwallet_it != usagstatus_idx.end()) && 
	// 			(cardwallet_it->contract_ac == asset_contract_ac)
	// 			, "player has less than 3 available cards. Please ensure min. 3 cards available for selection of asset contract: \'" + asset_contract_ac.to_string() + "\'");
	// 		card_ids.emplace_back(cardwallet_it->card_id);
	// 	}

	// 	return card_ids;
	// }


	// -----------------------------------------------------------------------------------------------------------------------
	static vector<uint64_t> checkget_3_selected_cards(const name& player, const name& asset_contract_ac) {
		// create an empty vector of card
		vector<uint64_t> card_ids{};

		// read the `cardwallet` table & collect 3 selected cards of `asset_contract_ac`
		cardwallet_index cardwallet_table("gpkbatescrow"_n, player.value);
		auto usagstatus_idx = cardwallet_table.get_index<"byusagstatus"_n>();
		auto cardwallet_it = usagstatus_idx.find("selected"_n.value);

		check( (cardwallet_it != usagstatus_idx.end()) &&
				(cardwallet_it->contract_ac == asset_contract_ac)
				, "player has no cards of asset contract: \'" + asset_contract_ac.to_string() + "\' selected for selection.");
		
		// capture the 1st card 
		card_ids.emplace_back(cardwallet_it->card_id);	
		
		// capture the 2 more cards 
		while(card_ids.size() < 3) {
			++cardwallet_it;
			check( (cardwallet_it != usagstatus_idx.end()) && 
				(cardwallet_it->contract_ac == asset_contract_ac)
				, "player has less than 3 selected cards. Please ensure min. 3 cards selected for selection of asset contract: \'" + asset_contract_ac.to_string() + "\'");
			card_ids.emplace_back(cardwallet_it->card_id);
		}

		return card_ids;
	}

/**
 	compute game fee if cards are of a type-1
	1. type-1: exotic | base | A or B
	2. // todo
 */
	static asset compute_gamefee( 
									const name& asset_contract_ac,
									const name& owner,
									const name& player,
									const vector<uint64_t> card_ids
								) 
	{

		// auto selected_card_ids = checkget_3_selected_cards(const name& player, const name& asset_contract_ac);

		sassets assets(asset_contract_ac, owner.value);

		// initialize game_fee
		asset game_fee = asset(0, symbol("WAX", 8));		// represents "0.00000000 WAX"
		// initialize game_fee_list
		vector<asset> game_fee_list = {};

		for(auto&& card_id : card_ids) {
			auto idx = assets.find(card_id);

			check(idx != assets.end(), "Asset with id " + std::to_string(card_id) + " not found or not yours");
			check (idx->author == "gpk.topps"_n, "Asset is not from this author");				// for WAX Mainnet
			// check (idx->author == "gpkbattlesco"_n, "Asset is not from this author");				// for WAX Testnet

			auto mdata = json::parse(idx->mdata);

			// type-1
			if( (idx->category == "exotic"_n)  && (mdata["variant"] == "base") && ( (mdata["quality"] == "a") || (mdata["quality"] == "b") ) ) {
				game_fee_list.emplace_back(asset(100000000, symbol("WAX", 8)));			// inserts "1.00000000 WAX"
			}
			// add other combinations of cards like prism, etc..
		}

		// check the game_fee_list has to be of size 3 in order to proceed further for playing
		check(game_fee_list.size() == 3, "the game_fee can\'t be computed because the 3 selected cards by " + player.to_string() + " are of incompatible types than what is defined for this game.");

		// if all the game_fees for the cards are equal
		// basically, check if the vector has all the elements as same.
		if ( std::all_of(game_fee_list.begin(), game_fee_list.end(), [&game_fee_list](auto i){return i == game_fee_list[0];}) ) 
		{
		    game_fee = game_fee_list.front();
		}

		return game_fee;
	}

	// -----------------------------------------------------------------------------------------------------------------------
	// check cards' category, quality, variant & 2A,1B or 1A,2B before/after the transfer to the contract
	static void check_cards_type( const name& asset_contract_ac,
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
			check (idx->author == "gpk.topps"_n, "Asset is not from this author");				// for WAX Mainnet
			// check (idx->author == "gpkbattlesco"_n, "Asset is not from this author");				// for WAX Testnet
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

	}

	// -----------------------------------------------------------------------------------------------------------------------
	// check cards' category, quality, variant & 2A,1B or 1A,2B before/after the transfer to the contract
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
			check (idx->author == "gpk.topps"_n, "Asset is not from this author");				// for WAX Mainnet
			// check (idx->author == "gpkbattlesco"_n, "Asset is not from this author");				// for WAX Testnet
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

		return card_ids_type;
	}
	// -----------------------------------------------------------------------------------------------------------------------
	static void check_quantity( const asset& quantity ) {
		check(quantity.is_valid(), "invalid quantity");
		check(quantity.amount > 0, "must withdraw positive quantity");
		check(quantity.symbol == symbol("WAX", 8), "symbol precision mismatch");
	}

private:
	// ======================================================================================================================
	// scope - <asset_contract_name> like simpleassets, atomicassets
	TABLE config {
		uint64_t id;
		name author;
		name category;
		string variant;
		string quality;
		asset game_fee;

		auto primary_key() const { return id; }
		uint64_t by_author() const { return author.value; }
	};

	using config_index = multi_index<"configs"_n, config,
								indexed_by< "byauthor"_n, const_mem_fun<config, uint64_t, &config::by_author>>
								>;

	// ======================================================================================================================
	// scope - self
	TABLE ongamestat {
		uint64_t game_id;
		name player_1;
		name player_2;
		asset game_fee;
		name p1_gfee_deducted;		// y or n
		name p2_gfee_deducted;		// y or n
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
		name asset_contract_ac;
		vector<name> players_list;

		auto primary_key() const { return asset_contract_ac.value; }
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
	inline name find_game_result(const checksum256& random_value) {
	    auto byte_array = random_value.extract_as_byte_array();

	    uint64_t random_int = 0;
	    for (int i = 0; i < 8; i++) {
	        random_int <<= 8;
	        random_int |= (uint64_t)byte_array[i];
	    }
	    
	    name res = ""_n;
	    uint64_t num1 = random_int % 2;				// produces either 0 or 1
		if(num1 == 0)
			res = "a"_n;
		else if(num1 == 1)
			res = "b"_n;

		return res;
	}

	// -----------------------------------------------------------------------------------------------------------------------
	// // get random index from list (of players, of cards)
	template<typename T>
	inline uint64_t get_random_indexfrmlist(const checksum256& random_value, vector<T> list) {
		//cast the random_value to a smaller number
		// this means creating random no. upto max., where, num1 <  max (not included)
		// i.e. num1 < N in case of list
		// Here, it is generalized for cards, players both. 
		// So, in case of cards shuffling would happen in out of 3 cards. So, 0 to 2 indices allowed
		// In case of remaining players in `pair2player` ACTION, 0 to N-1, N is the size of remaining_players_list.
	    uint64_t max_value = list.size();
	
	    auto byte_array = random_value.extract_as_byte_array();

	    uint64_t random_int = 0;
	    for (int i = 0; i < 8; i++) {
	        random_int <<= 8;
	        random_int |= (uint64_t)byte_array[i];
	    }
	    
	    uint64_t num1 = random_int % max_value;

	    return num1;
	}


	// -----------------------------------------------------------------------------------------------------------------------
	// remove player from the players_list, if present
	inline void rem_player(const name& asset_contract_ac, 
								const name& player) {
		players_index players_table(get_self(), get_self().value);
		auto players_it = players_table.find(asset_contract_ac.value);

		if (players_it != players_table.end())
		{
			auto vec_it = std::find(players_it->players_list.begin(), players_it->players_list.end(), player);
			if(vec_it != players_it->players_list.end()) {
				players_table.modify(players_it, get_self(), [&](auto& row) {
					row.players_list.erase(vec_it);
				});
			}
	}
		
	}

	// -----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	bool hasDuplicates(const std::vector<T>& arr) {
	    for (std::size_t i = 0; i < arr.size(); ++i) {
	        for (std::size_t j = i + 1; j < arr.size(); ++j) {
	            if (arr[i] == arr[j])
	                return true;
	        }
	    }
	    return false;
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
	// Adding inline action for `moergameinfo` action in the same contract 
	void moer_game_info(uint64_t game_id, const string& message);


};