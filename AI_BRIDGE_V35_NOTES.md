# AI Bridge V35 — Evidence-based opening role guard

V34 over-blocked Gallade because it hard-preserved any active that merely had Trick Room/Wide Guard present in the shared move summary.

V35 keeps the general role system but changes hard preservation to require positive scoring evidence:

- Fake Out hard-preserve only if Fake Out is positive.
- Trick Room/Tailwind hard-preserve only if the setup move is positive.
- Wide Guard hard-preserve only if Wide Guard is positive into spread/Fairy pressure.
- Otherwise support moves become soft opportunity-cost penalties, not hard vetoes.

Expected result: in Alcremie + Grimmsnarl lead, Mienshao can still be preserved when Fake Out is real, but Gallade should no longer be blocked from switching into Lucario just because Trick Room/Wide Guard exists with neutral score.
