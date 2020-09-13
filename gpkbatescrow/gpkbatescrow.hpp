#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
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

using std::string;
using std::vector;

using json = nlohmann::json;


CONTRACT gpkbatescrow : public contract
{
private:
	// const symbol gamefee_token_symbol;
	const name game_contract_ac;
	// const name asset_contract_ac;

public:
	using contract::contract;

	gpkbatescrow(name receiver, name code, datastream<const char*> ds) : 
				contract(receiver, code, ds), 
				// gamefee_token_symbol("WAX", 4),
				game_contract_ac("gpkbattlesco"_n)
				// asset_contract_ac("simpleassets"_n) 
				{}

	ACTION transferbypl( const name& player,
						const name& asset_contract_ac,
						const vector<uint64_t> card_ids,
						const string& memo );

	ACTION setgstatus( const name& player, 
						uint64_t card_id,
						const name& status );

	ACTION withdrawbypl( const name& player,
							const name& asset_contract_ac,
							uint64_t card_id );

	using setgstatus_action  = action_wrapper<"setgstatus"_n, &gpkbatescrow::setgstatus>;

	// -----------------------------------------------------------------------------------------------------------------------
	// check card's category, quality, variant & 2A,1B or 1A,2B before/after the transfer to the contract
	static void check_cards_type( const name& asset_contract_ac,
								const name& owner,
								const vector<uint64_t> card_ids,
								const name& category,
								const string& variant ) 
	{
		check(card_ids.size() == 3, "the card_ids list chosen must be of size 3");
		vector<string> cardtypes{};

		sassets assets(asset_contract_ac, owner.value);

		for(auto&& card_id : card_ids) {
			auto idx = assets.find(card_id);

			check(idx != assets.end(), "Asset with id " + std::to_string(card_id) + " not found or not yours");
			check (idx->author == "gpk.topps"_n, "Asset is not from this author");
			check(idx->category == category, "The asset id\'s category must be exotic.");

			auto mdata_1 = json::parse(idx->mdata);
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
			((cardtype_1 == "b") && (cardtype_2 == "b") && (cardtype_3 == "a")) || 
			
			, "the cards chosen are of different combination than (2A,1B) OR (1A,2B)."
			);
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