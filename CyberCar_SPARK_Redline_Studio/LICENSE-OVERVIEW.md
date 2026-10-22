# License map and redistribution guide

This repository intentionally uses different licenses for different kinds of work. Do not assume that a single license covers every file in the archive.

## 1. Software and ordinary authored documentation

`LICENSE` contains the complete Apache License, Version 2.0. It is intended for the firmware, helper code and authored project documentation only insofar as the relevant contributors own or are authorized to license those materials. Keep `LICENSE` and `NOTICE.md` with redistributed copies. Third-party libraries retain their own licenses.

## 2. Carrier PCB hardware design

`LICENSE-HARDWARE` contains the unmodified CERN Open Hardware Licence Version 2 — Strongly Reciprocal (`CERN-OHL-S-2.0`). The intended scope is stated in `LICENSE-HARDWARE-NOTICE.md`. CERN-OHL-S-2.0 has source-sharing obligations for modified covered designs and associated products; review the complete official license before using or conveying a design/product.

## 3. Images and other media

Face images, team photographs, the Spark name/logo, third-party icons, fonts and other artwork are not automatically covered by the code or PCB licenses. Review `LICENSE-IMAGES-AND-ASSETS.md`. Only the rights holder can grant permission. The presence of an image in a project ZIP does not establish that it is free to reuse.

## 4. Safety and warranty

All code, CAD files, diagrams and example settings are provided for educational prototyping. They have not been certified as safe, reliable, fit for a particular purpose, electrically compliant, or fabrication-ready. Follow the relevant license disclaimers, independently review the design, and verify electrical ratings before building or operating hardware. This notice is not legal advice.

## 5. Before a public release

- Obtain sign-off from everyone who owns a contribution to code, PCB, diagrams or branding.
- Audit imported code, third-party libraries, image rights and module/vendor designs.
- Preserve the appropriate license, attribution and notice files.
- For the hardware license, publish or otherwise provide complete source and an appropriate stable Source Location as required by CERN-OHL-S-2.0.
- Do not imply that CERN, Espressif, LOLIN, Arduino or component manufacturers endorse SPARK Team or this project.
