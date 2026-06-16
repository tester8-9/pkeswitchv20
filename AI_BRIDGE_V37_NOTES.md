# AI Bridge V37 — opening Fake Out slot guard cleanup

V37 is based on the full v36 package, with one targeted but general fix:

- V36 could still choose to switch out a Fake Out lead at the true opening if Fake Out scored neutral instead of positive.
- V37 preserves the active slot if that active Pokémon is a common Fake Out user and Fake Out is present in the opening score window.
- This is not partner-global: a partner having Fake Out does not block the other active slot from switching.
- This is not permanent: the guard is tied to the early candidate-score window, so Fake Out users can still switch later after Fake Out is no longer a turn-one resource.

Expected Ian behavior with Alcremie + Grimmsnarl lead:

- Mienshao -> Lucario should be blocked at opening.
- Gallade -> Lucario should be allowed if the score supports it.
- Trick Room/Wide Guard/priority remain evidence-based soft role factors, not blanket hard vetoes.
