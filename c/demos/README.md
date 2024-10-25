# QiSpace-SDK-Demo-C-App

A functionality demo based in C to demonstrate the usage of the MASQ APIs. For the best running experience, run the demo on an Ubuntu platform.

## **Overview:**
- **MASQ** provides **Asymmetric Encryption** functionality. The two main functions of MASQ are **Key Exchange and Digital Signature** using a pair of public and private keys.

## **Installation and Preparation**
1. Download or checkout this git repository to the local machine.
2. Get the QiSpace MASQ SDK from Quantropi.
3. Place the `libmasq` folder containing MASQ libraries for your OS at the top of this repository. 
    - `libmasq`: should contain all the contents from `ds` and `kem` from QiSpace MASQ SDK .`ds` and `kem` contain the needed Big Number libraries.

## **Getting Started**

> Note: Consider running the makefile commands individually if an error occurs. After a demo finishes, consider running `make clean` for consistent demo execution.

*For more information, please refer Makefile  for details*

## **Core Functionalities**

### MASQ KEM Demo `demo_masq_kem`:
- Initialize MASQ using one of:

  1) QiSpace NGen random callback functions from QiSpace QEEP SDK, or
  2) User-provided random callback functions

- Generate Keypair using the MASQ KEM API
- Encrypt & Decrypt the message using MASQ KEM APIs

Test the below supported Key-Exchange algorithms:

`ALG = hppk kyber`

The supported Big Number libraries are `BN = qtp gmp`

> Note: Kyber library does not depend on any Big Number library. 

The Defaults used for the demo are `ALG=hppk` for NIST Security level 3, using `BN=qtp` and QiSpace NGen as the Initialization API to generate random numbers.

```
make masq_kem
```

To test other specific algorithms, run
```
make masq_kem ALG=<ALG> BN=<BN>
```

### MASQ DS Demo `demo_masq_ds`:
- Initialize MASQ using one of:

  1) QiSpace NGen random callback functions from QiSpace QEEP SDK, or 
  2) User-provided random callback functions

- Generate Keypair to sign the message
- Sign the message
- Verify the message and signature using MASQ DS API

Test the below supported Digital Signature algorithms:

`ALG = ghppk hppk dilithium`

The supported Big Number libraries are `BN = qtp gmp`

> Note: Dilithium library does not depend on any Big Number library.

The Defaults used for the demo are `ALG=ghppk` for NIST Security level 5, using `BN=qtp` and QiSpace NGen as the Initialization API to generate random numbers.

```
make masq_ds
```

To test other specific algorithms, run
```
make masq_ds ALG=<ALG> BN=<BN>
```
 
## **License** 
See [LICENSE](LICENSE). Copyright © Quantropi, Inc 
 
For more general information about MASQ APIs see Quantropi documentation site.