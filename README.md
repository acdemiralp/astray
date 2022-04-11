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
  - Toggle `ASTRAY_BUILD_APPLICATIONS` for building the example applications.
  - Toggle `ASTRAY_BUILD_TESTS` for building the tests.
  - Toggle `ASTRAY_USE_FFMPEG` for video support.
  - Toggle `ASTRAY_USE_MPI` for MPI support. The build will ask for the location of MPI upon enabling this option.
  - Toggle `ASTRAY_DEVICE_SYSTEM` for CUDA/OMP/TBB support. The build will ask for the location of the respective library upon enabling this option.
  - Remember to generate or run `bootstrap.bat` after changes. You can ignore the cmake developer errors as long as generation is successful.
  
### Next Steps
- Compute the FTLE and the LCS of null geodesics.
- Precompute the Christoffel symbols into a 4D image. Sample the image instead of computing the symbols to accelerate ray tracing.
- More metrics and motions.
- Viewer application.
- Documentation.

### Potential Issues
- Check if rotations of the observer modify the environment correctly.
- Check if error controllers are transmitted to the device correctly.
- Complete the tests.