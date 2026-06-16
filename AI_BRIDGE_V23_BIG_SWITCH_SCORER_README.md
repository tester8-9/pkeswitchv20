# AI Bridge v23 - Gen-IV-inspired + VGC switch scorer

This build is a larger switch-AI implementation step.  It keeps the engine-level candidate bridge from v20-v22, but replaces the narrow Fairy/Fire tuning with a broader scoring model.

## What v23 adds

- Gen-IV-inspired send-in scoring:
  - 4x = 160
  - 2x = 80
  - 1x = 40
  - 1/2x = 20
  - 1/4x = 10
  - 0x = 0
- Row-specific switch scoring, not first acceptable row.
- Unique visible threat collection, not repeated-row overcounting.
- Switch-in defensive fit vs each visible threat.
- Switch-in offensive pressure using STAB and known move-threat profiles.
- Cinderace/Libero and Greninja/Protean-style move-threat profiles.
- Alcremie/Fairy vs Lucario tuning.
- Cinderace + Grimmsnarl mixed-board safety.
- Pangoro/Gallade/Conkeldurr/Mienshao Fairy-board penalties.
- Fake Out turn-one hard block for Mienshao.
- Positive Fake Out/tempo move penalty.
- Wide Guard preservation penalty on spread/Fairy boards, without hard-blocking switching.
- Priority pressure check: if the current active has positive priority pressure into a target, switching is penalized.
- Future-role approximation for valuable low-HP preservation logic; true HP reading is not yet mapped, so this is still inferred from move-score/danger context.

## Notes

This build still relies on the game-created switch candidates, so it should not invent illegal switches.  It does not yet read exact HP, exact current abilities, exact volatile status, or Dynamax state from memory.  Those are future layers.

Look for these log labels:

```text
v23_team
v23_score
v23_gate
switch_policy_v23
candidate_score_after_switch_policy_v23
```
