# V21 - Best-row switch selection + Fairy pivot tuning

V20 worked, but it selected the first allowed switch row. That meant action 2 / Conkeldurr could be enabled before action 3 / Lucario was scored as the better Fairy-answer.

V21 changes mode 7 to score all available switch candidate rows first, then enables only the highest scoring row above threshold.

It also tunes Lucario vs Fairy/Cinderace boards:
- Lucario gets a much larger reward into visible Alcremie.
- Lucario gets a moderate reward into Grimmsnarl.
- Cinderace still penalizes Lucario, but less harshly if a Fairy threat is also visible.
- Pure Cinderace boards remain strongly bad for Lucario.

Look for log labels:
- v21_team
- v21_score
- v21_gate
- switch_policy_v21 best_row
- candidate_score_after_switch_policy_v21
