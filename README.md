# Cson

_A very simple JSON reader for C_

![image](https://github.com/user-attachments/assets/06c267a5-15fd-4746-add2-c4b57a9e95f5)

![image](https://github.com/user-attachments/assets/fad4d955-82d0-4fb8-ab66-bb3a88bf262f)

**Better syntax error reporting**

![image](https://github.com/user-attachments/assets/278b00ec-6f61-423c-b5bd-18de82f73be4)

## Implementing a new parser

The new parser, parses the json as a tree, making it easier to travel over the path specified by the user and prevent key conflicts.

![image](https://github.com/user-attachments/assets/de709f36-688f-40bd-b159-526011baf26b)

## Testing

**We have some examples to you:**

basic one:

```bash
$ make basic
$ ./basic
```

testing the new parser:

```bash
$ make st
$ ./st
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

## Next improvements

- Better error reporting at parser level
- Fix the formatter scaping strings
- Tokenize, lex and parse negative numbers
- Generate a libray from Cson

## Next steps

- Create a first unstable release

## Disclaimer

> [!NOTE]
> This is a working in progress. It's not ready yet for any kind of use.
