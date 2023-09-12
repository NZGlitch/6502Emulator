### MOS 6052 CPU Emulator by NZGlitch

This is a 'pet' project to implement a fully working MOS 6502 Emulator. 
I've always been interested in learning C++ and have a keen interest in
retro computing so this seemed like a good idea.

---
## *** WORK IN PROGRESS ***

**TODO's** (In no particular order)
 - Implement addAccumulator testson CPU for Decimal mode
 - Implement instructions: 
   - ADC, CMP, CPX, CPY, SBC, BRK, RTI
 - Write tests for BaseInstruction class
 - Refactor older instructions to match new architecture
    - JUMP, LOAD, SHIFT, STACK, STORE, TRANSFER
 - Cleanup gtest warnings on compile
 - Make command line executable that runs a binary file
 - Licence & copyright
---

### Getting Started

The emulator is not yet ready to run as a stand alone application as there are still
instructions to implement. You can run the Test suite and execute a test program - 
see [E6502Test/src/test_program.cpp](E6502Test/src/test_program.cpp) for an example.

**Acknowledgements**

Special thanks to Dave Poo and his video series on his implementation of the 6502. 
While my approach is quite different, Dave's videos got me onto a good start -
you can see them on his Youtube channel here: https://www.youtube.com/@DavePoo

Also a super shout out to [Klaus2m5](https://www.github.com/klaus2m5) for his amazing
functional test suites.

Other resources used include:
* [Ulitmate C64 regerence @ Pagetable](https://www.pagetable.com/c64ref/6502)
* [obelisk.me 6502 reference site via Wayback Machine](https://web.archive.org/web/20210909190432/http://www.obelisk.me.uk/6502/)
* [The C64 wiki](https://www.c64-wiki.com/wiki/Main_Page)
* [Turbo Macro Pro](http://turbo.style64.org)
* [6502.org](http://www.6502.org/)

Developed in VisualStudio 2019 on Windows 11

© Copyright 2023 all rights reserved.