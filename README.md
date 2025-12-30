# Markdown Transpiler

Program for lexing, parsing and rendering given Markdown file into an
HTML file.

Uses _some_ C++20 features.

### Feature subset & support

This Transpiler supports:

- ### headings
- **emphasis**
- _italics_
- > _quotes_ with **parsing**
- code spans:

```cpp
std::cout << "Hello World!\n";
```

- `inline code snippets`
- **nested _inline_ elements**
- lists
  - nested lists
    - more nested lists
- [links](https://tfoedy.com)
- images:

![images](https://avatars.githubusercontent.com/u/61157294?v=4&size=64)

This project loosely follows the CommonMark specification.
Not everything is supported, as I never intended for it to be fully feature-complete.
There are likely numerous edge cases this transpiler fails to account for,
and it is by no means a real implementation of the specification.

### Usage

Command line:

```
markdowntranspiler <input_markdown_filename> (output_filename) [--only-body] [--no-styling]
```

- --only-body - render HTML with just the body part
- --no-styling - render HTML without any styling

It's also possible to drag a Markdown file onto the `markdowntranspiler` binary in a GUI file manager.

### Building

#### Requirements:

- C++20 compiler
- CMake (>= 3.31)

---

You can either use the `cmake-gui` tool to generate a solution for your desired IDE (E.g. on Windows for Visual Studio) or, in the root project directory do:

```
> mkdir build
> cmake -B build
> make -C build
```

which will compile the binary `markdowntranspiler` in the `build` directory.

### License

This project uses the [`MIT license`](LICENSE).

Done as the _final_ project for the 1st semester at my University.
