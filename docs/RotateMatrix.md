# Rotate Matrix

- default Matrix

```Matrix
[ 1, 0, 0, 0 ]
[ 0, 1, 0, 0 ]
[ 0, 0, 1, 0 ] 
[ 0, 0, 0, 1 ]
```

- matrix array point table

```Matrix
[ 00, 04, 08, 12 ]
[ 01, 05, 09, 13 ]
[ 02, 06, 10, 14 ] 
[ 03, 07, 11, 15 ]
```

- sx, sy, sz -> zoom XYZ (scale start 1.0x)
- tx, ty, tz -> Translation XYZ (range -1 -> 1)

```Matrix
[ sx,  0,  0, tx ]
[  0, sy,  0, ty ]
[  0,  0, sz, tz ] 
[  0,  0,  0,  1 ]
```

- rotate X axis
to lock X axis, set `[0, 0]` is 1

```Matrix
[  1,  0,  0,  0 ]
[  0,  c, -s,  0 ]
[  0,  s,  c,  0 ]
[  0,  0,  0,  1 ]
```

- rotate Y;
to lock Y axis, set `[1, 1]` is 1

```Matrix
[  c,  0,  s,  0 ]
[  0,  1,  0,  0 ]
[ -s,  0,  c,  0 ]
[  0,  0,  0,  1 ]
```

- rotate Z;
tolock Z axis, set `[2, 2]` is 1

```Matrix
[  c, -s,  0,  0 ]
[  s,  c,  0,  0 ]
[  0,  0,  1,  0 ]
[  0,  0,  0,  1 ]
```

---

## result matrix

After set rotate translate and zoom, mix all table into this matrix.

```Matrix
[  r * s, r * s,  r * s,  tx ]
[  r * s, r * s,  r * s,  ty ]
[  r * s, r * s,  r * s,  tz ]
[       0,    0,      0,   1 ]
```
