# Qeep-SDK-Demo-Java-APP

A Java-based Android demo app demonstrating the functionality and usage of QiSpace QEEP and SEQUR APIs. It was developed on Android API 32
Note: The app was developed on an emulated Google Pixel 2 running Android 12 (Pixel 2 API 31), so UI elements may be inaccurate on devices with different displays. Furthermore, internet access is required for most aspects of the demo app.

## **Overview:**
- #### SEQUR: Demonstrates the functionality of SEQUR API to decode the Quantum Entropy (or any other QiSpace-retrieved payload) and the QiSpace QE REST API
- #### QEEP: Shows an instance of QEEP API encryption/deccryption usage and the accompanying QiSpace QK REST API
- #### MASQ: The MASQ API is currently unsupported on Android devices and will be coming soon. If urgent access is necessary, contact Quantropi directly.

Starting on the landing page, the user can then access additonal demo pages that illustrate the SEQUR, QEEP and eventually MASQ APIs via buttons and text boxes. To navigate to another page, the back button may be pressed to return to the landing page.

## **Installation and Preparation:**
1. Download or checkout this git repository to a local machine
2. Place the `prebuilt` folder containing the QEEP and SEQUR libraries into `/QiSpace_SDK_Lib` directory
    * Note: If the libraries given to you are not in a `prebuilt` folder, create one under `/QiSpace_SDK_Lib` (i.e: `home/QiSpace_SDK_Lib/prebuilt`) and place the contents into the newly created `prebuilt` folder. 
3. Copy the `QiSpace_Info.java` class into `app/src/main/java/com/example/qispace_sdk_java_demo/` directory  
4. Open and setup Android Studio with Android API 32 and above SDK and Pixel 2 API 31 and above as device
    * Note: If any of the files or folders mentioned above are not available to you, please contact Quantropi support team.

## **Getting Started:**
1. Open this repository within **Android Studio**.
2. Build and run the app.

## **Core Functionalities of this Demo:**
### SEQUR `SequrActivity`:
_Assume the API token is already established_
- SETUP SUBKEY: establish the subscriber key using QiSpace REST API and unpack it
- GET QE: obtain the Quantum Entropy from QiSpace REST API and unpack it
- Generate RANDOM STRING: call Pseudo Quantum RaNdom Data API to generate random string from the QE as seed
*Note: This demo page shows an instance of QE (Quantum Entropy) decoding, but it can be applied to QK as well*

### QEEP `QeepActivity`:
_Assume the sub_key and token are already established_
- GET QK: retrieve, convert and decode the Qeep Key from QiSpace API
- ENCRYPT: Apply QEEP API to encrypt the message using the QK
- DECRYPT: Apply QEEP API to decrypt the encrypted message with same QK

### MASQ `MasqActivity` (Coming soon):

## **Libraries used: No installation required**
- Android API 32
- Android volley
- org.json

## **License:**
See the [LICENSE] (LICENSE) file. Copyright © Quantropi, Inc

For more information regarding the QiSpace APIs or the QEEP/SEQUR/pQrnd APIs, see official Quantropi API documentation


