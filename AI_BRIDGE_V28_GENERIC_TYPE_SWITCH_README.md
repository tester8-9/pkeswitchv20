# AI Bridge V28 — generalized type-matchup switch scorer

V28 responds to the issue where v27 still did not switch and was too centered around the Cinderace/Grimmsnarl/Lucario test case.

Changes:

- Uses a tighter fresh-record window so stale targets leave the board faster after a KO/replacement.
- Scores visible board types generally, not only specific species names.
- Rewards any Steel/Poison switch-in into Fairy pressure, not just Lucario.
- Rewards switching out of Fairy-weak active Pokémon into Fairy-resistant switch-ins.
- Penalizes switch-ins weak to visible Fire/Ground/Psychic/Flying pressure by type, not by species.
- Lowers the approval threshold from 40 to 15.
- Once a single best row is approved, commits hard to that row with score 1,000,000.

This still only boosts the single best approved row. It does not blindly enable all switch candidates.
