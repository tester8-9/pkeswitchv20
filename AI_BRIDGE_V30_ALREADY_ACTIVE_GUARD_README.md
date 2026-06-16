# AI Bridge V30 — already-active switch guard + Fake Out row-order cleanup

V29 looks successful mechanically: it resolved the board, selected the best switch row, and wrote the final commit fields.

V30 keeps the v29 full type/final-commit scorer but adds two cleanup fixes:

1. Already-active switch-in guard.
   If the proposed switch-in species is already visible in the fresh CPU-side target records, the bridge treats it as already active and blocks that row. This prevents repeated attempts to switch into Lucario after Lucario is already on the field.

2. Fake Out row-order cleanup.
   V29 consumed `opening_fakeout_once` after the first candidate row, which could block action 2 but still allow action 3 in the same window. V30 removes that artifact. Fake Out is now a hard block only when the active Pokémon's Fake Out is actually scoring positively; otherwise it is just handled by the normal opportunity-cost score.

Use the same config as v29.
