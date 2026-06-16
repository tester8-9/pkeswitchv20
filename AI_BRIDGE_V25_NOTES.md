# AI Bridge v25 — dynamic switch commit score

This is based on the v24 stability-cleanup branch, with one gameplay change:

- The best-row scorer still chooses the highest-scoring switch row.
- Instead of always writing the fixed config `switch_score` (usually 115), v25 scales the candidate commit score upward only when the matchup score is very strong.

Reason: the v23 log showed Gallade -> Lucario scored very highly on an Alcremie + Grimmsnarl board, but if the in-game action did not commit, a fixed 115 candidate score may have been too low to beat the normal move action.

Dynamic target score:
- score >= 360 -> candidate score 170
- score >= 280 -> candidate score 155
- score >= 200 -> candidate score 140
- score >= 120 -> candidate score 128
- otherwise use the config switch_score floor

This still avoids the old proof-mode 1,000,000 switch spam.
