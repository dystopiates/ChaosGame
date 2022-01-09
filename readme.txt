There are two projects I ended up making: one in Python, one in C. Each folder (C, Python) has another readme.txt within it, as well as some comments inside the main code describing what's going on and how to run it. Do not expect clean, well-designed code - this is very exploratory code, where I'm messing around with new ideas, not final, professional-quality code.

The first project is Python-only, and it generates and displays fractals, entirely in Python.

Eventually, that was too slow, so I wrote some C code to generate the fractals, and edited the
Python project to only read those saved fractals and render / display an image.

Currently, it's set up to run python-only. Check the readme.txt in the Python folder. Any comments
starting with 2 ## are comments I added just for this upload. Other than that, there's lots of
WIP or commented-out code left behind. It's an art project, not professional code.


The C version does the work of computing and pixel-counting the fractals much more efficiently,
and is several orders of magnitude faster than the Python code. It also takes significantly less memory.

With the C code, I generate the points to a file, then read that file back to the C code to count
the pixels, and finally run *that* final output into the Python code, which turns the pixel counts
into an image to save. Check the comments in the main file of the C project for more info.
