# AI Bridge V32 — dynamic bench / party occupancy memory

V31's destination lockout was intentionally too blunt. It stopped repeated Lucario commits, but it did not solve the real problem: after a switch, the CPU-side bench contents change, so action IDs must be mapped to the current bench, not the original trainer-party order.

V32 keeps ping-pong switching legal, but updates the bridge's predicted party occupancy after each committed switch.

Core change:

- Initialize the enemy roster from the resolved trainer team.
- Mark the first two living slots as active.
- Treat action IDs as compact bench indices: action 2 = first current bench option, action 3 = second, etc.
- When a switch is committed, swap the active species out and mark the chosen bench species active.
- Future switch scoring uses the updated bench list.

This means if action 3 initially means Lucario, and Mienshao switches into Lucario, action 3 should no longer be treated as Lucario afterward. It should refer to whatever is now the second current bench option.

Expected log markers:

- `v32_roster_init`
- `v32_team ... action2=... action3=...`
- `v32_roster_commit`
- `v32_score`
- `switch_policy_v32`
