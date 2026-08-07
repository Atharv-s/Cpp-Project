Run on OnlineGDB (3 steps)
Go to onlinegdb.com
Set language to C++ (top-right dropdown)
Paste the entire file → click Run

You'll get an interactive menu in the terminal:

============================================================
  IMAGE <-> BINARY CONVERTER
  Pure C++11 | Works on OnlineGDB, Replit, Godbolt
============================================================

  [1] DEMO    - Auto round-trip test (no input needed)
  [2] ENCODE  - Your image (Base64) -> Binary text
  [3] DECODE  - Binary text -> Your image (Base64)
  [4] MANUAL  - Type binary bits, inspect byte values
  [0] EXIT

  What each mode does

[1] DEMO — Press 1, hit Enter. No input needed. It runs a full encode→decode round-trip on a built-in PNG, shows you the binary output, verifies byte-for-byte identity, and prints a data:image/png;base64,... URI you can paste in a browser to see the image.

[2] ENCODE — Convert your own image. Get your image as Base64 first (Python one-liner below), paste it in, get back the full binary text. Type DEMO to use the built-in test PNG.

[3] DECODE — Paste binary text (e.g., output from mode 2), type END to finish, get back a Base64 data URI and raw Base64 you can paste in a browser to recover the image.

[4] MANUAL — Educational mode. Type one 8-bit group at a time (10001001, 01010000, …). After each one it shows you the hex and decimal value. Type show to see all bytes so far, done to finish.

Get Base64 from your image (for mode 2)
python
# Python — run this locally, paste output into OnlineGDB
python3 -c "import base64; print(base64.b64encode(open('photo.png','rb').read()).decode())"

Or use base64.guru/converter/encode/image online.
