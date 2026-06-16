# AI Bridge V34 — general active-role preservation + v33 memory baseline

V34 keeps the v33 dynamic bench/roster memory system, but generalizes the previous Mienshao-specific opening guard into a role-based active-slot guard.

Instead of hardcoding only Mienshao, V34 preserves the active slot if that active Pokémon itself has immediate opening value such as Fake Out, Wide Guard/spread protection, Trick Room/Tailwind, or priority tempo. This still allows the partner slot to switch.

Expected behavior:
- Alcremie + Grimmsnarl lead: if Mienshao has immediate tempo, preserve Mienshao and allow Gallade/Lucario pivot.
- Other teams: preserve the active Pokémon only when its own role indicates immediate value, not because its partner has Fake Out.
- Lucario staying in versus Cinderace is still scored normally; if alternatives are worse, staying in can be correct.
