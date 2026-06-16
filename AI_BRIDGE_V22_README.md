# AI Bridge v22 - Fake Out / mixed Cinderace-Fairy tuning

This build keeps the v21 best-row scorer, but tightens two unsafe cases discovered in the v21 log:

1. First Mienshao/Fake Out window protection
   - If Mienshao has Fake Out in the current move-score window and the bridge has not yet forced a switch, v22 blocks switching out of Mienshao.
   - This prevents wasting turn-one Fake Out tempo when the native move score is neutral.

2. Lucario into Cinderace + generic Fairy tuning
   - Lucario is still rewarded into Alcremie.
   - Lucario is only lightly rewarded into Grimmsnarl when Cinderace is absent.
   - Lucario is punished into Cinderace + Grimmsnarl boards, because Cinderace Fire pressure is too important.
   - Pure Cinderace boards strongly punish Lucario.

The switch acceptance threshold was also raised from 20 to 40 so borderline pivots do not pass just because they are barely positive.

Use the same [ai_bridge] config as v20/v21.
