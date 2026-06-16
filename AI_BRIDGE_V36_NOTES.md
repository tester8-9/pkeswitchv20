# AI Bridge V36 — evidence role guard + protected story ace switch block

V36 is based on V35 evidence-role-guard.

Main changes:

- Keeps V35's evidence-based role guard: Trick Room / Tailwind / Wide Guard / priority no longer hard-block merely because the active Pokémon knows them. They hard-preserve only when the current scoring window gives positive evidence; otherwise they are soft opportunity-cost signals.
- Adds protected story-ace switch prevention for gym/champion/story boss fights. The AI bridge will not voluntarily switch into the original final party slot for those fights, which prevents premature Machamp/Charizard/etc. appearances and dialogue timing issues. Native forced send-in after a KO is left alone.
- Adds a Perish Song detection log (`v36_note perish_song_seen`) but does not claim full count==1 switching yet. Exact Perish counter reading still needs a memory mapping step.

Look for:
- `v36_gate skip action=... reason=protected_story_ace`
- `v36_score`
- `switch_policy_v36`
- `v36_note perish_song_seen`
