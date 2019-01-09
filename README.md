# REDHAWK Basic Components rh.DataConverter
 
## Description

The rh.DataConverter component is used to convert between BULKIO data types in
REDHAWK. With proper configuration, the rh.DataConverter component can convert
between any of the following data types; Char, Octet, Unsigned Short, Short,
Float and Double. The rh.DataConverter component is also capable of converting
real data into complex data, and similarly complex data to real data.

## Branches and Tags

All REDHAWK core assets use the same branching and tagging policy. Upon release,
the `master` branch is rebased with the specific commit released, and that
commit is tagged with the asset version number. For example, the commit released
as version 1.0.0 is tagged with `1.0.0`.

Development branches (i.e. `develop` or `develop-X.X`, where *X.X* is an asset
version number) contain the latest unreleased development code for the specified
version. If no version is specified (i.e. `develop`), the branch contains the
latest unreleased development code for the latest released version.

## REDHAWK Version Compatibility

| Asset Version | Minimum REDHAWK Version Required |
| ------------- | -------------------------------- |
| 5.x           | 2.2                              |
| 4.x           | 2.0                              |
| 3.x           | 1.10                             |
| 2.x           | 1.8                              |

## Installation Instructions
This asset requires the rh.fftlib shared library. This shared library must be
installed in order to build and run this asset. To build from source, run the
`build.sh` script found at the top level directory. To install to $SDRROOT, run
`build.sh install`. Note: root privileges (`sudo`) may be required to install.

## Asset Use
To use the rh.DataConverter connect an input port to the desired input data stream that matches the input data type. Connect the output to the output port of the desired output type. Only one inport port can be active at a time, but the data can be converted to multiple output types at the same time. If scaling is desired, fixed point output scaling can be enabled by the 'scaleOutput' boolean. For floating point scaling or normalization, use the 'floatingPointRange' and 'normalize_floating_point' properties. The rh.DataConverter can also do real to complex and complex to real transformations. The output type (real/complex/passthrough) is controlled with the 'outputType' property. If a transform is required, the properties controlling that transform are specified in the 'transformProperties' property. 
## Copyrights

This work is protected by Copyright. Please refer to the
[Copyright File](COPYRIGHT) for updated copyright information.

## License

REDHAWK Basic Components rh.DataConverter is licensed under the GNU General
Public License (GPL).

