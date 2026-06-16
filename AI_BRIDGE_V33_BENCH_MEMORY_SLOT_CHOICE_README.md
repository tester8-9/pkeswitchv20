# V33 — bench memory + slot choice correction

V33 fixes two v32 issues seen in the user's log:

1. `switchin_already_visible` no longer relies on fresh move-target records. Those records can contain stale or previous-active species, such as Mienshao appearing after it has already switched out. V33 uses the predicted roster state instead.

2. Mienshao is preserved in the opening Fairy/spread board if no switch has yet been committed. This lets the partner slot, such as Gallade, be the one that pivots into Lucario. This is active-owned and does not block the partner from switching just because Mienshao has Fake Out/Wide Guard utility.

Expected log labels:
- `v33_score blocked: preserve_opening_mienshao`
- `v33_score blocked: switchin_already_visible` only when the predicted roster says the switch-in is active
- `switch_policy_v33`
- `v33_roster_commit`
