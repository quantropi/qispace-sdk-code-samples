# QiSpace-SDK-Demo-Java-App

A Java-based Android demo app demonstrating the functionality and usage of QiSpace QEEP and SEQUR APIs. Developed on Android API 32.

Note: The app was developed on an emulated Google Pixel 2 running Android 12 (Pixel 2 API 31). UI elements may differ on devices with different displays. Internet access is required to properly run this demo app.

## **Overview**
- #### SEQUR: Demonstrates functionality of SEQUR API to decode Quantum Entropy (or any other QiSpace-retrieved payload) and the QiSpace QE REST API.
- #### QEEP: Shows an instance of QEEP API encryption/decryption usage and the accompanying QiSpace QK REST API.
- #### MASQ: Coming soon. The MASQ API is currently unsupported on Android devices. If urgent access is necessary, contact Quantropi support directly.

On the landing page, a user can access additional demo pages that illustrate the SEQUR, QEEP, and MASQ (coming soon) APIs via buttons and text boxes. The back button may be pressed from any page to return to the landing page.

## **Installation and Preparation**
1. Download this git repository to a local machine.
2. Place the `prebuilt` folder containing the QEEP and SEQUR libraries into `/QiSpace_SDK_Lib` directory.
    * Note: If the libraries given to you are not in a `prebuilt` folder, create one under `/QiSpace_SDK_Lib` (i.e: `home/QiSpace_SDK_Lib/prebuilt`) and place the contents into the newly created `prebuilt` folder. 
3. Copy the `QiSpace_Info.java` class into `app/src/main/java/com/example/qispace_sdk_java_demo/` directory.  
4. Open and setup Android Studio with Android API 32 and above SDK and Pixel 2 API 31 and above as device.
    * Note: If any of the files or folders mentioned above are not available to you, please contact Quantropi support team.

## **Getting Started**
1. Open this repository within **Android Studio**.
2. Build and run the app.

## **Core Functionalities**
### SEQUR `SequrActivity`:
_Assume the API token is already established._
- SETUP SUBKEY: Establish the subscriber key using QiSpace REST API and unpack.
- GET QE: Obtain Quantum Entropy from QiSpace REST API and unpack.
- Generate RANDOM STRING: call PQRND API to generate random string from the QE as seed.

*Note: This demo page only shows an instance of QE decoding, but it can also be applied to QK*

### QEEP `QeepActivity`:
_Assume the sub_key and token are already established._
- GET QK: Retrieve, convert and decode the Qeep Key from QiSpace API.
- ENCRYPT: Apply QEEP API to encrypt the message using the QK.
- DECRYPT: Apply QEEP API to decrypt the encrypted message with same QK.

### MASQ `MasqActivity` (Coming soon):

## **Libraries**
*No installation required*
- Android API 32
- Android volley
- org.json

## **License**
See [LICENSE](LICENSE). Copyright © Quantropi, Inc

Refer to documentation site for more general information about SEQUR, QEEP and PQRND APIs as well as QiSpace REST API.


