# AI Bridge v28 — generic type-first switch scorer

v28 is built on the v27/v25 project structure but removes the most narrow species-specific board logic.

Main changes:

- Uses a generic type-first pivot layer instead of hard-coding Cinderace/Grimmsnarl/Lucario behavior.
- Steel gets a general reward into Fairy boards; Fighting/Dark/Psychic actives are treated as exposed to Fairy pressure.
- Fire/Ground/Fighting threat pressure generically punishes Steel pivots.
- Replacement target reset is more aggressive, so stale board records should drop faster when a new opposing species appears.
- Positive Fake Out after the opening is a caution penalty, not a permanent hard veto.
- Active-owned setup/priority is a softer delay instead of a large universal penalty.
- Best-row threshold relaxed from 40 to 20.

This should apply to general teams/type matchups rather than one specific Cinderace/Grimmsnarl/Lucario board.
