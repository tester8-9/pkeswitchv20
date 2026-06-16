# AI Bridge V32 — dynamic roster / compact bench action mapping

V31-style loop guards were too blunt for the user's intent. Ping-pong switching can be valid in VGC; the actual problem was that the bridge treated switch action IDs as fixed party slots, so after Lucario was already active it could still evaluate action 3 as if it meant Lucario.

V32 keeps the v30/v29 full type scorer and final commit, but changes the action-to-species mapping:

- Maintains a predicted CPU roster for the resolved trainer team.
- Initializes active Pokémon from the first two party slots, then corrects with observed active species.
- Treats action IDs as compact bench choices:
  - action 2 = first currently benched option
  - action 3 = second currently benched option
  - action 4 = third currently benched option
- After a committed switch, updates the predicted active/bench split.
- Future scoring uses the updated bench list, so it no longer keeps thinking Lucario is benched after Lucario was switched in.

This is not an anti-ping-pong rule. Switching back and forth is still allowed if the roster memory says the destination is actually in the back and the matchup score approves it.

Expected log markers:

- `v32_roster_init`
- `v32_roster_commit`
- `v32_gate skip action=... reason=no_predicted_bench_mapping`
- `switch_policy_v32`
- `candidate_score_after_switch_policy_v32`
