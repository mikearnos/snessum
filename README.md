# snessum

If you've ever opened up a SNES save game file (.srm) in a hex editor and changed how much money or weapons you have, you'll find out that when played back that the game has erased your save. Due to the volitility of the battery backed data, most/all SNES games use a checksum routine to ensure the save data is uncorrupted due to a dead battery. Most games simply add all the bytes together to get a final number.

The source is here but also a compiled binary for anyone who wants a quick checksum fix.

```
Chrono Trigger
Illusion of Gaia
Metroid 3
Secret of Evermore
Secret of Mana
Seiken Densetsu 3
Tales of Phantasia
Terranigma
Star Ocean
Zelda 3
```
