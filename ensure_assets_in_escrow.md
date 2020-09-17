gbuser111121 - 2b1a

* create using Anchor wallet
* transfer from `gpkbattlesco` owner to `gbuser111111` user
```console
cleosw push action simpleassets transfer '{"from": "gpkbattlesco", "to": "gbuser111121", "assetids": ["100000000007716", "100000000007728", "100000000007729"], "memo": "transfer cards for gpk.battles playing"}' -p gpkbattlesco@active
```

* transfer (2a1b or 1a2b cards combo) from `gbuser111111` user to `gpkbatescrow` contract
```console
cleosw push action simpleassets transfer '{"from": "gbuser111121", "to": "gpkbatescrow", "assetids": ["100000000007716", "100000000007728", "100000000007729"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111121@active
```

* add details in `cardwallet` & `players` tables by giving details of player - `gbuser111111`, asset contract_ac & the transferred cards (above) to `gpkbatescrow` contract
```console
cleosw push action gpkbatescrow transferbypl '{"player": "gbuser111121", "asset_contract_ac": "simpleassets", "card_ids": ["100000000007716", "100000000007728", "100000000007729"], "memo": "transfer cards for gpk.battles playing"}' -p gpkbatescrow@active
```