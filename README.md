# Cson

_A very simple JSON reader for C_

![image](https://github.com/user-attachments/assets/fad4d955-82d0-4fb8-ab66-bb3a88bf262f)

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

## Disclaimer

> [!NOTE]
> This is a working in progress. It's not ready yet for any kind of use.
