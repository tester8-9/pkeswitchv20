# AI Bridge v20 — generalized matchup switch scorer

V20 is a larger tuning/generalization step after v19.

Changes:
- Added proper logging name for Alcremie (species 869) and generated many more species display names from the species/type table.
- Kept the dynamic trainer-team resolver introduced earlier. The scorer does not rely on Ian-only names except for a few role/safety rules.
- Normalized switch scoring by unique visible threat species instead of counting every per-move/per-target scoring row. This reduces the over-penalty problem from v19.
- Preserves the Fake Out tempo rule for Mienshao-like lead behavior.
- Keeps move-threat logic for Libero/Protean-style species such as Cinderace/Greninja, so current temporary type changes do not trick the AI into unsafe switches.
- Keeps Lucario-into-Cinderace and Pangoro-into-Fairy safety rails.

Recommended first config is the same v19-style switch_policy_mode = 7 config. Look for v20_score and v20_gate lines in yuzu_log.txt.
