# AI Bridge v20 - generalized balanced switch scorer

V20 is a larger switch-AI step, not a narrow Ian-only probe.

Changes:
- Adds broad species display names, including Alcremie (species 869), so logs no longer print `869/` blank.
- Keeps Alcremie typed as Fairy.
- Uses a generated trainer-team table from the current trainer dump to infer enemy party slots across normal trainers.
- Normalizes switch scoring by unique visible enemy species instead of counting every move-score record repeatedly.
- Keeps runtime target detection, so your team changing still changes the visible target species in the scorer.
- Adds more balanced VGC-style switch scoring: defensive improvement, offensive pressure, move opportunity cost, role/future value, and anti-bad-switch penalties.

Limitations:
- If you substantially edit trainer_poke/trainer_data teams later, rebuild/regenerate this project so the embedded trainer-team table matches your current files.
- HP and exact move ownership are still not fully mapped, so low-HP preservation is represented through role/future-value heuristics, not exact HP-aware switching yet.
