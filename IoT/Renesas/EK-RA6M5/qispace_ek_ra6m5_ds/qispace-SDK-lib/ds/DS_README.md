# MASQ-DS
Welcome to the MASQ-DS library 

## **Dependencies**
For Premium version,to build and test MASQ-DS, the following libraries are required.
Ensure they are both available under the `lib_<ALG>` packages:

- **A Big Number library** 
- **QiSpace QEEP PQRND** 

> **Dilithium** library does not depend on qeep and any Big Number library.

## **Contents**
This Release contains the following packages:
- lib_`<ALG>`
- demo (Not available for IoT package)

### *lib_`<ALG>`*

where `<ALG>` is one of the below supported algorithms:

`ALG = ghppk dilithium`

Base version supports `dilithium` algorithms.

Premium version supports `ghppk` algorithms.

Inside this package you will find the MASQ DS library for specified target and the Big Number libraries for `QTP` and `GMP`.
    
Included header files:
- `masq_ds.h` - QiSpace MASQ DS APIs
- `masq_ds_error.h` - QiSpace MASQ DS API Return Code 


### *demo*
We have demo available for 3 host platforms(Windows, MacOS and Linux). Please follow the README file from the `demo` folder to run demo.

