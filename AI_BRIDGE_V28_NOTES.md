# AI Bridge V28 — generalized type/move scorer

V28 is not tuned around the specific names Cinderace, Grimmsnarl, or Lucario.
It keeps species move-threat profiles for special cases such as Libero/Protean,
but the switch decision is now driven primarily by generalized type relationships:

- visible target species are read from runtime move evaluation objects;
- each target contributes threat move types from its species typing / known move profile;
- Steel/Poison pivots are rewarded into Fairy pressure generally, not only Lucario into Alcremie;
- switch-ins weak to visible threat move types are penalized generally;
- neutral bulky pivots get some value but less than true type-advantage pivots;
- commit score is dynamic so strong approved switches can actually beat regular moves.

If no current board species is detected in the log, the scorer will not invent a switch.
