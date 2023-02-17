# mini-scanf

Minimal `scanf()` implementation for embedded projects. All this happiness 150 lines `:)`

## Motivation
Recently I had the opportunity to implement firmware for systems with limited resources (`6502`,`8080`). No one expects speed from them, so the amount of addressable memory has become the main limitation.

I looked around the internet for some stripped down `scanf()` implementations but none I really liked. Then I decided to develop my own minimal `scanf()`/`sscanf()`.

## Compatibility

I did not implement this tool in strict accordance with the standard. Rather, it parodies functions from glibc.

Supported control characters:
```text
%c  - matches a character or a sequence of characters
%s  - matches a sequence of non-whitespace characters (a string) 
%[] - matches a non-empty sequence of character from set of characters. 
%d  - matches a decimal integer. 
%u  - same as %d (You can enable ignoring the sign. nostd)
```

Mechanisms are also implemented:
```text
// ?  - control characters
// @  - any character
// .. - etc

%12..0? - max field width modifier
%*?     - input suppression
%[@..]  - read selected characters
%[^@..] - read until a character is encountered
```

It's completely standalone without any external dependencies.

## Configuration:
```text
C_SSCANF // Comment out `#define` in order to use code like scanf
LENSCANS // The number of characters processed in the `%[]` construct
```

## Credits

Project inspired by the work of [mini-printf](https://github.com/mludvig/mini-printf)