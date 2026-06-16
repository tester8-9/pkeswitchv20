# AI Bridge v23 — Gen IV + Showdown-inspired switch scorer

This version is a larger implementation step rather than a small probe.

Main additions:

- Best-row selection is retained from v21/v22.
- Gen IV-inspired send-in scoring buckets are used as a switch-in heuristic:
  4x = 160, 2x = 80, 1x = 40, 1/2x = 20, 1/4x = 10, immune = 0.
- Showdown-inspired switch legality discipline is used conceptually:
  avoid choosing already-active, fainted, or duplicate simultaneous switch slots.
- Move-score gating remains active: good current moves, especially positive Fake Out pressure, discourage switching.
- Threat-profile scoring is still runtime/species based, with Cinderace/Libero-style threat handling.
- The scorer evaluates all available switch rows, then enables only the best legal/approved row.
- Wide Guard/ally support is treated as role value, but it does not automatically forbid switching if the board position clearly favors a pivot.
- Alcremie/Fairy-board handling remains; Lucario is rewarded into clean Fairy pressure but still punished into Cinderace/Fire pressure.

Important limitation:

This is still not reading every exact move from every visible Pokemon's runtime move list. It uses species/type/move-threat profiles plus the live move-score summary from the engine. It should generalize to other trainers when the embedded trainer-team table contains their party order and the species/type table knows those species.

Recommended test:

1. Build this project through GitHub Actions.
2. Install the new Sword artifact / subsdk9.
3. Use the config.toml included in this zip, or paste its [ai_bridge] section into your existing config.
4. Test Ian first, then one different trainer if possible.
5. Upload yuzu_log.txt and report whether any switch looked smart or dumb.

Expected log tags:

- v23_team
- v23_score
- v23_gate blocked
- v23_gate allowed
- switch_policy_v23 best_row
- candidate_score_after_switch_policy_v23
