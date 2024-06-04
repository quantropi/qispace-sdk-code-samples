# QiSpace-SDK-Demo-Kotlin-App

A Kotlin-based Android demo application demonstrating the functionality and usage of QiSpace QEEP and SEQUR APIs. Developed on Android API 32.

Note: The application was developed on an emulated Google Pixel 2 running Android 12 (Pixel 2 API 31). UI elements may differ on devices with different displays. Internet access is required to properly run this app.

## **Overview**
- #### SEQUR: Demonstrates the functionality of SEQUR API to decode the Quantum Entropy (or any other QiSpace-retrieved payload) and the QiSpace QE REST API.
- #### QEEP: Shows an instance of QEEP API encryption/decryption usage and the accompanying QiSpace QK REST API.
- #### MASQ: Coming soon. The MASQ API is currently unsupported on Android devices. If urgent access is necessary, contact Quantropi support directly.

On the landing page, a user can access additional demo pages that illustrate the SEQUR, QEEP, and MASQ (coming soon) APIs via buttons and text boxes. The back button may be pressed from any page to return to the landing page.

## **Installation and Preparation**
1. Download this git repository to a local machine.
2. Copy the `QiSpace_Info.kt`class into `app/src/main/kotlin/com/example/demoqispaceapp/`.
3. Place the `prebuilt` folder containing the QEEP and SEQUR libraries into `QiSpace_SDK_Lib` folder.
    * Note: If the libraries given to you are not in a `prebuilt` folder, create one under `/QiSpace_SDK_Lib` (i.e: `home/QiSpace_SDK_Lib/prebuilt`) and place the contents into the newly created `prebuilt` folder. 
4. Open and set up Android Studio with Android API 32 SDK and Pixel 2 API 31 as a device.
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

### MASQ `MasqActivity` (Coming soon):

## **Libraries**
*No installation required*
- Android API 32
- Android volley
- org.json

## **License**
See [LICENSE](LICENSE). Copyright © Quantropi, Inc

Refer to the documentation site for more general information about SEQUR, QEEP and PQRND APIs as well as QiSpace REST API.


