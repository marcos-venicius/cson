# Cson

_A simple and stupid JSON decoder in C_

![image](https://github.com/user-attachments/assets/b01b8aff-bc7b-4d1d-afe1-a3c922db81ca)


## Testing

**We have some examples to you:**

basic one:

```bash
$ make basic
$ ./basic
```

nested_objects one:

```bash
$ make nested_objects
$ ./nested_objects
```

nested_arrays one:

```bash
$ make nested_arrays
$ ./nested_arrays
```

nested_stuff one:

```bash
$ make nested_stuff
$ ./nested_stuff
```

## Disclaimer

> [!NOTE]
> This is a working in progress. It's not ready for any kind of use.

> [!WARNING]
> We have some warnings in the compiler that needs some attention.

## Next improvements

- Possible bugs
    - Add tests to the parser to check if it's working well
- Better error reporting
- Objects key naming conflicts with the internal way of storing the path until the object

For instance:

```json
{
    "a.b.c": 1,
    "a": {
        "b": {
            "c": 2
        }
    }
}
```

Will conflict, the first one will be returned.

## Idea of implementation to support nested objects

I'll have an array of size `n`.
`n` will be the estimated amount of tokens (it's based on the amount of `COLON_CSON_TOKEN` in the json file).

Every time the parser encounter a `{`, this will add a new prefix, that is the key to this object and a `.`.

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

The idea is similar to the idea of nested objects, the difference is that to each item in the array, the prefix will be `[<index>]`.

Everything will be saved in a contiguous array in memory, and the search for any key will be `O(n)`, `n` being the amount of keys in the whole json.

**Terrible, but, I think it will work**

