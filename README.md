GO Game with Japanese rules
________________________________________________

This is an implementation of Japanese GO game for two players, that uses ASCII characters to represent the board in command line.
It uses Conio library.

The game is intended for two players playing on the same computer.


Rule details and simplifications
_____________________________________

Automatic scoring is implemented, but since Japanese scoring rules are close to impossible to compute accurately (for those who are interested, here is a research paper on the subject: https://www.oipaz.net/Carta.pdf),
my algorithm is simplified and it often breaks. Using it is not recommended.

Program does not detect when a game is finished, so score computing is only used by pressing the ,,f" key.
However the program enforces the Ko rule (no move may cause return to the previous state of the board) and automatically detects and removes ,,dead" stones.


Controls
_____________________________________

Controls are given in the upper left corner of the game window.

Placing stones is done by moving cursor on the board and pressing "i" key after selecting a place.

When starting a new game, player will be promoted for board size.
One of the options is ,,custom". It allows player to privide custom board size.
If the board is too large to be displayed in full, a limited scope will be displayed and will move together with the cursor.


License
_____________________________________

Software is thereby released under MIT Licence. See opensource.org for details. Copy of the license below:

Copyright 2023 Mateusz Nurczyński

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
