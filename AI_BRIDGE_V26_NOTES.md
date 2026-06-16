# AI Bridge V26 — fresh board + active-only Fake Out guard

V26 implements the requested behavior after the v25 log:

- Keeps only a *single* opening Fake Out guard, and only for the active Pokémon that is actually switching out.
- Allows the other active Pokémon to switch even if its partner has Fake Out.
- Uses fresh visible targets from the newest move-scoring window so stale Cinderace records do not keep vetoing Lucario after Cinderace leaves and Alcremie appears.
- Delays switching if the Pokémon that would switch out appears to have active setup value, such as Trick Room or Tailwind, or positive priority-move pressure.
- Keeps Lucario strongly rewarded into Alcremie/Fairy boards, but still penalized into pure Cinderace boards.

Expected labels:

- `v26_score`
- `v26_score blocked: opening_fakeout_once`
- `v26_gate blocked`
- `switch_policy_v26`
- `candidate_score_after_switch_policy_v26`

Keep `score_survey_dump_state = false` for normal testing.
