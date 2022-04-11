### Astray
A performance-portable relativistic ray tracing library.

### Requirements
- CMake
- Git
- Optional: CUDA
- Optional: MPI

### Getting Started
- Clone the repository.
- Run `bootstrap.[bat|sh]`. This will install all dependencies except CUDA and MPI, and create the project under the `./build` directory.
- Optional: Run cmake on the `./build` directory.
  - Toggle `ASTRAY_BUILD_TESTS` for building tests.
  - Toggle `ASTRAY_USE_FFMPEG` for video support.
  - Toggle `ASTRAY_USE_MPI` for MPI support. The build will ask for the location of MPI upon enabling this option.
  - Toggle `ASTRAY_DEVICE_SYSTEM` for CUDA/OMP/TBB support. The build will ask for the location of the respective library upon enabling this option.
  - Remember to generate or run `bootstrap.bat` after changes.

### Next Steps
- Core and math tests.
- Documentation.