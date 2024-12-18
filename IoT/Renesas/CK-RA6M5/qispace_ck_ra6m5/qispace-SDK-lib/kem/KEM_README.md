# MASQ-KEM
Welcome to the MASQ-KEM library 

## **Dependencies**
For Premium version, to build and test MASQ-KEM, the following libraries are required. Ensure they are both available under the `lib_<ALG>` packages:

- **A Big Number library** 
- **QiSpace QEEP PQRND** 

> **Kyber** library does not depend on Qeep and any Big Number library.

## **Contents**
This Release contains the following packages:
- lib_`<ALG>`
- demo (Not available for IoT package)

### *lib_`<ALG>`*

where `<ALG>` is one of the below supported algorithms:

`ALG = hppk kyber`

Base version supports `kyber` algorithms.

Premium version supports `hppk` algorithms.

Inside Premium package you will find the MASQ KEM library for specified target and the Big Number libraries for `QTP` and `GMP`.
    
Included header files:
- `masq_kem.h` - QiSpace MASQ KEM APIs

- `masq_kem_error.h` - QiSpace MASQ KEM API Return Code


### *demo*
We have demo available for 3 host platforms(Windows, MacOS and Linux). Please follow the README file from the `demo` folder to run demo.
