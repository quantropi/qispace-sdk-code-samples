# Qeep SDK Demo for C++ Application

A functionality demo based in C++ to demonstrate the usage of the MASQ APIs. For the best running experience, run the demo on Ubuntu platform.

## **Overview**
- **MASQ** provides **Asymmetric Encryption** functionality. The two main functions of MASQ are **Key Exchange** and **Digital Signature** using a pair of public and private keys.

## **Installation and Preparation**
1. Download or checkout `qispace-sdk-code-sample` git repository to the local machine.
2. Contact Quantropi to get the latest QiSpace MASQ SDK libraries.
3. Create the `libmasq` folder containing MASQ libraries depending on the platform at the top of this repository. The `libmasq` should contain all the contents from `ds` and `kem` from QiSpace MASQ SDK. The `ds` and `kem` contain the needed Big Number libraries.
  
## **Core Functionalities**

### MASQ KEM:

MASQ KEM demo is implemented in `src/demo_masq_kem.c` file.

MASQ KEM supports below Key-Exchange algorithms:

`ALG = hppk kyber`

The supported Big Number libraries are `BN = qtp gmp`

> Note: Kyber library does not depend on any Big Number library. 

- Initialize MASQ using one of:

  1) QiSpace NGen random callback functions from QiSpace QEEP SDK, or
  2) User-provided random callback functions

    The Defaults used for the demo are `ALG=hppk` for NIST Security level 3, using `BN=qtp` and QiSpace NGen as the Initialization API to generate random numbers.

- Generate Keypair using the MASQ KEM API
- Encrypt & Decrypt the message using MASQ KEM APIs


### MASQ DS:

MASQ DS demo is implemented in `src/demo_masq_ds.c` file.

MASQ DS supports below Digital Signature algorithms:

`ALG = ghppk hppk dilithium`

The supported Big Number libraries are `BN = qtp gmp`

> Note: Dilithium library does not depend on any Big Number library.

- Initialize MASQ using one of:

  1) QiSpace NGen random callback functions from QiSpace QEEP SDK, or 
  2) User-provided random callback functions

    The Defaults used for the demo are `ALG=ghppk` for NIST Security level 5, using `BN=qtp` and QiSpace NGen as the Initialization API to generate random numbers.

- Generate Keypair to sign the message
- Sign the message
- Verify the message and signature using MASQ DS API

## **Running the Demo** 

- To build and run the MASQ KEM demo, run below command in console
```
make masq_kem
```

- To test other specific KEM algorithms, run
```
make masq_kem ALG=<ALG> BN=<BN>
```

- To build and run the MASQ DS demo, run below command in console

```
make masq_ds
```

- To test other specific DS algorithms, run
```
make masq_ds ALG=<ALG> BN=<BN>
```

> Note: Consider running the makefile commands individually if an error occurs. After a demo finishes, consider running `make clean` for consistent demo execution.

## **License** 
See the [LICENSE](LICENSE) file. Copyright © Quantropi, Inc 
 
For more general information about MASQ APIs, see Quantropi documentation site.
