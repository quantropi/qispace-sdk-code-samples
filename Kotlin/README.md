# QiSpace-SDK-Demo-Kotlin-App

A Kotlin-based Android demo application demonstrating the functionality and usage of QiSpace QEEP, SEQUR and MASQ APIs. Developed on Android API 35.

Note: The application was developed on an emulated Google Pixel 9 running Android 15 (API 35). UI elements may differ on devices with different displays. Internet access is required to properly run this app.

## **Overview**
- #### SEQUR: Demonstrates the functionality of SEQUR API to decode the Quantum Entropy (or any other QiSpace-retrieved payload) and the QiSpace QE REST API.
- #### QEEP: Shows an instance of QEEP API encryption/decryption usage and the accompanying QiSpace QK REST API.
- #### MASQ: MASQ provides Asymmetric Encryption functionality. The two main functions of MASQ are Key Exchange and Digital Signature using a pair of public and private keys.

On the landing page, a user can access additional demo pages that illustrate the SEQUR, QEEP, and MASQ (Available Now!) APIs via buttons and text boxes. The back button may be pressed from any page to return to the landing page.

## **Installation and Preparation**
1. Download this git repository to a local machine.

2. Copy the `QiSpace_Info.kt`class into `app/src/main/kotlin/com/example/demoqispaceapp/`. A sample class may look like so:
    ```kotlin
    package com.example.demoqispaceapp

    class QiSpace_Info {
        companion object {
            val QISPACE_ENTERPRISE_BASE_URL = "http://your.kds/kds"
            val QISPACE_ENTERPRISE_ACCESS_TOKEN = "your_device_token"
        }
    }
    ```
3. Unzip the QiSpace SDK Premium Release zip package containing the QEEP, SEQUR and MASQ libraries into `QiSpace_SDK_Lib` folder.

4. Open and set up Android Studio with Android API 35 SDK and Pixel 9 API 35 as a device.
    * Note: If any of the files or folders mentioned above are not available, please contact the Quantropi support team.

## **Getting Started**
Build and run the application with Android Studio.

## **Core Functionalities**
### SEQUR `SequrActivity`:
_Assume the API token is already established._
- SETUP SUBKEY: Establish the subscriber key using QiSpace REST API and unpack.
- GET QE: Obtain Quantum Entropy from QiSpace REST API and unpack.
- Generate RANDOM STRING: Call PQRND API to generate a random string from QE as a seed.

*Note: This demo page shows an instance of QE decoding, but it can also be applied to QK *

### QEEP `QeepActivity`:
_Assume the sub_key and token are already established._
- GET QK: Retrieve, convert and decode Quantum Key from QiSpace API.
- ENCRYPT: Apply QEEP API to encrypt the message using the QK.
- DECRYPT: Apply QEEP API to decrypt the encrypted message with the same QK.

### MASQ-KEM `MasqActivity` :
- Keypair: Generate a public-private keypair from QiSpace MASQ KEM API.
- ENCAP: Apply MASQ KEM API to encapsulate the shared secret.
- DECAP: Apply MASQ KEM API to decapsulate the ciphertext to receive shared secret.

### MASQ-DS `MasqActivity` :
- Keypair: Generate a public-private keypair from QiSpace MASQ DS API.
- SIGN: Apply MASQ DS API to sign the message and generate the signature.
- VERIFY: Apply MASQ DS API to verify the message.

## **Libraries**
*No installation required*
- Android API 35
- Android volley
- org.json

## **License**
See [LICENSE](LICENSE). Copyright © Quantropi, Inc

Refer to the documentation site for more general information about SEQUR, QEEP, PQRND and MASQ APIs as well as QiSpace REST API.


