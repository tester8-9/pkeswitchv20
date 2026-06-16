# AI Bridge V26 — v25 structure + fresh-board/active-role changes

This package keeps the v25 repository/file structure and applies the v26 AI changes inside the v25 source tree.

Changes from v25:

- Keeps the full v25 build structure/workflow intact.
- Uses fresh move-score records to reduce stale-board issues after a player switch such as Cinderace -> Alcremie.
- Keeps the opening Fake Out guard active-owned and one-time only.
- Does not let a partner's Fake Out globally block the other active Pokemon from switching.
- Adds active-owned setup/priority delay logic for Trick Room, Tailwind, Wide Guard, and positive priority pressure.
- Keeps best-row candidate selection and dynamic commit scoring.

Use the same v25/v26 config block with `switch_policy_mode = 7`.
