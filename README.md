# Cson

_A simple and stupid JSON decoder in C_

## Running

```bash
make

# Debug
make DEBUG=1
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

- Handle escape strings
- Possible bugs
    - add tests to the parser to check if it's working well
- support nested objects
- support arrays
- better error reporting

## Idea of implementation to support nested objects

I'll have an array of size `n`.
`n` will be the estimated amount of tokens (it's based on the amount of `COLON_CSON_TOKEN` in the json file).

Everytime the parser encounter a `{`, this will add a new prefix, that is the key to this object and a `.`.

For example, imagine we have:

```json
{
    "testing": {
        "other_keys": "hello",
        "nested": {
            "hey": "Hi"
        }
    }
}
```

The key to the `testing -> nested -> hey` will be `testing.nested.hey`.

So, the user can just pass this key `testing.nested.hey` and grab the `hey` key as `string` in the json.

## Idea of implementation to support arrays

The idea is similar to the idea of nested objects, the differece is that to each item in the array, the prefix will be `[<index>]`.

Everything will be saved in a contiguous array in memory, and the search for any key will be `O(n)`, `n` being the amount of keys in the whole json.

**Terrible, but, I think it will work**
