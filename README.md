# fluffy-system

Reads the input from a USB piano. It's read by windows media api. The audio is
then output.

## Usage

1. Build
```
gcc -std=c11 -O3 -D_WIN32 main.c -o test.exe -lwinmm
```

2. Run

## Todo
- Add functionality for the peddles.
- Look into latency.