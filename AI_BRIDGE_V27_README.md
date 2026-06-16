# AI Bridge V27 - replacement-window reset + high-confidence commit

Built from the full v25/v26 project structure.

Changes from v26:

- Resets the move-score board window when a new target species appears after a full board was already built. This is meant to fix stale Cinderace records surviving after Cinderace is KO'd and Alcremie enters.
- Tightens the freshness window from 18 score records to 8.
- High-confidence switch rows now get a true commit score. If the row score is 200+, the selected row gets 1,000,000. This is not v5-style spam because only one scorer-approved best row is changed.
- Sets `move_records_consumed` after candidate evaluation so later candidate cycles start with fresh board data.

Expected test result for Cinderace+Grimmsnarl -> Cinderace KO -> Alcremie:

- Early Cinderace board should discourage Lucario.
- After Alcremie is in, stale Cinderace should drop from the current threat set.
- A strong Lucario/Fairy-board score should get a high commit score and be more likely to actually switch.
