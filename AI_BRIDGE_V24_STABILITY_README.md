# AI Bridge V24 — stability/log cleanup build

V24 keeps the V23 Gen-IV + VGC switch scoring behavior, but fixes the one technical issue seen in logs: harmless Yuzu `Unmapped Read8` spam from broad species probing against non-pointer packed values such as `0x00000021000000xx`.

Changes:

- Tightened runtime pointer validation to the validated battle-object heap region.
- Moved blind `species_probe` scanning behind `score_survey_dump_state = true`, so normal scoring no longer scans random structures.
- Kept the main V23 scoring behavior: Fake Out first-window block, Gen-IV type buckets, best-row selection, Fairy/Lucario scoring, Cinderace/Libero threat awareness, and dynamic trainer-team mapping.
- Log labels now use `v24_` so it is easy to confirm the new build is installed.

Use the same config as V23. For normal testing, keep:

```toml
score_survey_dump_state = false
```

Only turn that true for deep memory surveys, not gameplay testing.
