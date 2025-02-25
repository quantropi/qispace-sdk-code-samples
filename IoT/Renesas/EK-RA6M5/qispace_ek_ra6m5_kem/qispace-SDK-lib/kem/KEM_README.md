# MASQ-KEM
Welcome to the MASQ-KEM library 

## **Dependencies**
For Premium version, to build and test MASQ-KEM, the following libraries are required. Ensure they are both available under the `lib_<ALG>` packages:

- **A Big Number library** 
- **QiSpace QEEP PQRND** 

> **ML-KEM** library does not depend on QEEP and any Big Number library.

## **Contents**
This release contains the following packages:
- lib_`<ALG>`
- demo (Not available for IoT package)

### *lib_`<ALG>`*

where `<ALG>` is one of the below supported algorithms:
- hppk
- mlkem

Base version supports `mlkem` algorithm.

Premium version supports both `hppk` and `mlkem` algorithms.

Inside this premium package you will find the MASQ KEM library for both algorithms for specified target and the Big Number libraries for `QTP` and `GMP` for `hppk` algorithm.
    
Included header files:
- `masq_kem.h` - QiSpace MASQ KEM APIs
- `masq_kem_<ALG>.h` - QiSpace MASQ KEM APIs for specific `<ALG>`
- `masq_kem_error.h` - QiSpace MASQ KEM API Return Code


### *demo*
We have demo available for 3 host platforms (Windows, MacOS and Linux). Please follow the README file from the `demo` folder to run demo.
