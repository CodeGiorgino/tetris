# References
List of references used for this project:
- https://en.wikipedia.org/wiki/Tetromino
- https://en.cppreference.com/w/cpp/thread/mutex.html
- https://en.cppreference.com/w/cpp/thread/future.html
- https://stackoverflow.com/questions/421860/capture-characters-from-standard-input-without-waiting-for-enter-to-be-pressed
- https://en.cppreference.com/w/cpp/utility/format/spec.html

## Rotation
### Examples
```text
0deg:
■
■
@ ■
--------------------
(-2, 0)
(-1, 0)
( 0, 0) (0, 1)

90deg:
@ ■ ■
■
--------------------
(0, 0) (0, 1) (0, 2)
(1, 0)

180deg;
■ @
  ■
  ■
--------------------
(0, -1) (0, 0)
        (1, 0)
        (2, 0)
```

### Trasformation function
Defined the tetromino as a collection of offsets from the pivot point,
the rotated offset will be equal to `{ .y = -x, .x = y }`
