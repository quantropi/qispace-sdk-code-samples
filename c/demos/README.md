# QiSpace SDK Demo for C Application

A functionality demo based in C to demonstrate the usage of the MASQ APIs. For the best running experience, run the demo on Ubuntu platform.

## **Overview**
- **MASQ** provides **Asymmetric Encryption** functionality. The two main functions of MASQ are **Key Exchange** and **Digital Signature** using a pair of public and private keys.

## **Installation and Preparation**
1. Download or checkout `qispace-sdk-code-sample` git repository to the local machine.

2. Contact Quantropi to get the latest QiSpace MASQ SDK libraries.

3. Create the `libmasq` folder at the top of this repository.

4. Once the latest QiSpace MASQ SDK libraries has been obtained, `copy & paste` all the contents of `ds` and `kem` from QiSpace MASQ SDK according to the building platform. The `ds` and `kem` contain the needed Big Number libraries.
  
## **Core Functionalities**

### MASQ KEM:

MASQ KEM demo is implemented in `src/demo_masq_kem.c` file.

MASQ KEM supports below Key-Exchange algorithms:

`ALG = hppk mlkem`

The supported Big Number libraries are `BN = qtp gmp`

> Note: ML-KEM library does not depend on any Big Number library.

- Initialize MASQ using one of:

  1) QiSpace NGen random callback functions from QiSpace QEEP SDK, or
  2) User-provided random callback functions

    The Defaults used for this demo are `ALG=mlkem` for NIST Security level 3, using random number generator from inside ML-KEM library.

- Generate Keypair using the MASQ KEM API
- Encrypt & Decrypt the message using MASQ KEM APIs


### MASQ DS:

MASQ DS demo is implemented in `src/demo_masq_ds.c` file.

MASQ DS supports below Digital Signature algorithms:

`ALG = ghppk mldsa`

The supported Big Number libraries are `BN = qtp gmp`

> Note: ML-DSA library does not depend on any Big Number library.

- Initialize MASQ using one of:

  1) QiSpace NGen random callback functions from QiSpace QEEP SDK, or 
  2) User-provided random callback functions

    The Defaults used for this demo are `ALG=mldsa` for NIST Security level 5, using the demo provided random function.

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
See [LICENSE](LICENSE). Copyright © Quantropi, Inc 
 
For more general information about MASQ APIs see Quantropi documentation site.