# Cson

_A simple and stupid JSON decoder in C_

## Running

```bash
make
```

```bash
./cson
```

## Features

For now, this "library" only parse simple json files:

- strings
- nulls
- booleans
- numbers (as doubles)

This does not support nested objects (for now).

This does not support arrays (for now).

> [!NOTE]
> This is a working in progress. It's not ready for any kind of use.

> [!WARNING]
> We have some warnings in the compiler that needs some attention.

## Next improvements

- Possible bugs
    - fix memory issues (free memory, right allocation, etc)
    - add tests to the parser to check if it's working well
- support nested objects
- support arrays with
- better error reporting
- code cleanup
