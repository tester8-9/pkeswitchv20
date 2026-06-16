# AI Bridge V26 — v25-structure build

This package keeps the full v25 repository/file structure and applies the v26 logic changes into `source/program/patches/ai_bridge.hpp`.

Main changes from v25:

- Fresh-board filtering: move-score records now carry a sequence value and board scoring ignores stale records outside the newest slice. This is meant to prevent old Cinderace records from continuing to veto Lucario after the player switches Cinderace out into Alcremie.
- Opening Fake Out protection is consumed once. Mienshao is still protected at the true opening window, but it should not be permanently blocked from switching just because no earlier switch happened.
- Active-owned tempo checks: setup/tempo delays now depend on the active species being evaluated, so a partner's Fake Out should not block the other active Pokémon from switching.
- Active-owned role delay: Trick Room/Tailwind/Wide Guard/priority proxies penalize switching only for the active Pokémon that likely owns that role.
- Log labels are now `v26_score`, `v26_gate`, `v26_team`, and `switch_policy_v26`.

Use the same config block as the previous v26/v25 switch-policy builds.
