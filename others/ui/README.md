# GPK.Battles Game UI
## 0. After fetching cards from `simpleassets` contract, the cards are shown
<p align="center">
  <img src="0_players_cards_after_fetch.png" alt="" width="" height="">
</p>

## 1. After transferring all shown cards in `simpleassets` contract to `gpkbatescrow` contract
> NOTE: Here, many cards (no limit) can be transferred at once which saves CPU, NET resources & then the player can select cards subsequently.

<p align="center">
  <img src="1_players_cards_after_transfer.png" alt="" width="" height="">
</p>

## 2. After selecting cards (max. 3 of either `2a1b`/`1a2b`)
<p align="center">
  <img src="2_players_cards_after_sel_cards.png" alt="" width="" height="">
</p>

## 3. After pairing with a player
<p align="center">
  <img src="3_players_cards_display_after_pairing.png" alt="" width="" height="">
</p>

## 4. After playing
### A. Draw
<p align="center">
  <img src="4_1_players_cards_display_after_playing_draw.png" alt="" width="" height="">
</p>

### B. No Draw
> NOTE: Here, 2 ACTIONs are executed in sequence after 3 sec of first's success. `play` >> `disndcards`

<p align="center">
  <img src="4_2_players_cards_display_after_playing_nodraw.png" alt="" width="" height="">
</p>