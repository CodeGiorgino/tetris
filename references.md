- (Tetromino)[https://en.wikipedia.org/wiki/Tetromino]
- (std::mutex)[https://en.cppreference.com/w/cpp/thread/mutex.html]
- (std::future)[https://en.cppreference.com/w/cpp/thread/future.html]
- (Non blocking terminal input)[https://stackoverflow.com/questions/421860/capture-characters-from-standard-input-without-waiting-for-enter-to-be-pressed]
- (std::string format specs)[https://en.cppreference.com/w/cpp/utility/format/spec.html]

## Rotation
### Examples
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

### Trasformation function
```cxx
typedef std::pair<int, int> ivec2;

void rotate_point(ivec2& point) {
    auto [y, x] = point;
    point = {x, -y};
}
```
