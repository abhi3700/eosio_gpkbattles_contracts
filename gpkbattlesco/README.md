## TODO:
- if anything wrong going during testing. try to look if it's due to 

## NOTES
* The steps followed in the game is as follows:
	1. Match 2 players using `match2player` action
	2. Each player transfers fees using `token::transfer` action
	3. Each Player select 3 cards using `select3card` action
	4. if same card selected ==> Draw
	   if different cards selected ==> RNG is generated & then the winner gets the any 1 of 3 cards from loser.

* <u>Shuffle & Select</u>: This method is applied for: 
	- for a list of players, choose 2 for pairing them.
	- for a list of cards, choose 3 for auto-select of cards if the player doesn't select cards within 30 secs time.

* Post game:
	- transfer all the cards back.
		+ draw: nothing
		+ non-draw: 4 to winner & 2 to loser
	- remove the player name from `players_list` param in `players` table.