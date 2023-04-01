# A scuffed little Lisp interpreter written in C++

This is a little project I wrote to get experience in using C++ in about 5 hours. Don't use this for actual programs.

## Building

Building is done via GNU `make` using `g++`

```console
$ make
```

## Usage

After successful compilation, run the interpreter with:

```console
$ ./lisppp <input file>
```

e.g.:

```console
$ ./lisppp examples/hello_world.lisp
```

## License

Although very small, this project is licensed under the MIT License. See [LICENSE](./LICENSE) for copying conditions.
