## Testing
### For a User account
* `gbuser111111` generates random no. by passing a different signing_value per transaction
```console
$ cleosw push action orng.wax requestrand '["3700", "834736523243", "gbuser111111"]' -p gbuser111111@active
executed transaction: 01656e6492d7efa76a4688e426aac2a809fdcfacedab43bf1f268dc324327372  120 bytes  243 us
#      orng.wax <= orng.wax::requestrand        {"assoc_id":3700,"signing_value":"834736523243","caller":"gbuser111111"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- Here, when the action `requestrand` is executed, where `gbuser111111` consumes 376 bytes. View [here](https://wax-test.bloks.io/transaction/01656e6492d7efa76a4688e426aac2a809fdcfacedab43bf1f268dc324327372)
	- after few secs, this action - `setrand` is executed, where `gbuser111111` releases 256 bytes. View [here](https://wax-test.bloks.io/transaction/93b810514e11a97001d8565f31a54456eae44ab0d62c72969cf702e4bd5e68ec)

### For a Contract account
