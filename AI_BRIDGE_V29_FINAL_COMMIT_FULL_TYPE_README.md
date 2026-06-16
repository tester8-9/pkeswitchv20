# AI Bridge V29 — full dual-type matchup scorer + final switch commit

V29 addresses the failure shown in the v28 log where the bridge selected a switch candidate, enabled it, and raised its score, but the trainer still did not switch.

Core changes:

1. Full 18-type matchup model is retained.
   - TypeEffectiveness10Single covers all 18 attacking types against all 18 defending types.
   - TypeEffectiveness10 multiplies dual-type interactions, so dual typings produce 0x, 0.25x, 0.5x, 1x, 2x, and 4x outcomes.
   - The scorer evaluates current active typing, switch-in typing, and visible target/threat typing.

2. Row-specific switch scoring is retained.
   - Every native switch row is scored.
   - Only the best legal row is enabled.

3. Final-selection fields are now written.
   - Earlier builds showed `cand[action] enabled=1 score=300` but no in-game switch.
   - V29 writes the final fields exposed in the candidate-state dump:
     +0xF8 = final_valid
     +0xFC = final_score
     +0x100 = final_action
   - This is a high-confidence commit for the single best row only, not all switches.

4. Generic scoring remains type-first.
   - This is not special-case-only Cinderace/Grimmsnarl/Lucario logic.
   - Species move-threat profiles are only used to supplement type charts for Pokémon like Libero/Protean users where current typing can be misleading.

Look for these log markers:

- `v29_score`
- `v29_gate`
- `switch_policy_v29`
- `v29_final_commit`
- `candidate_score_after_switch_policy_v29` showing `final_valid=1`, `final_score=1000000`, and `final_action=<chosen action>`.
