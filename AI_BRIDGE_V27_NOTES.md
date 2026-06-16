# AI Bridge V27 — fresh candidate commit + stale-board cleanup

V27 fixes the main issue seen in the v26 log:

- v26 correctly identified Mienshao -> Lucario as the best switch after Alcremie appeared, but it scored too early while stale Cinderace records were still in the move summary.
- v27 builds the visible enemy board from the newest two unique opponent target species, so a Cinderace object left behind after Cinderace is KO'd or switched out should no longer keep suppressing Lucario once Alcremie is active.
- v27 also commits from the candidate_score hook if the move-score window is already ready. This lets the bridge use the later, cleaner candidate table after the replacement board has been rescored.
- High-confidence switch scores now commit more strongly: a score >= 200 targets at least 185 instead of 140.

Expected result for the Cinderace -> Alcremie test:
- early Cinderace+Grimmsnarl board: Lucario remains discouraged;
- after Cinderace leaves and Alcremie+Grimmsnarl are current: Lucario should score using those two current targets and commit more reliably.
