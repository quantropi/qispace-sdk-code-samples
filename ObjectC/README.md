# QiSpace-SDK-Demo-Objective-C-App

An iOS demo app made using Objective C and some C-based code to demonstrate the functionality and a usage of QiSpace QEEP and SEQUR APIs.

Note: This app was developed on an emulated iPhone 13. UI elements may differ on devices with different displays. Internet access is required to run this app.

## **Overview**
- #### SEQUR: Demonstrates the functionality of SEQUR API and the QiSpace QE API.
- #### QEEP: Shows an instance of QEEP API usage and the accompanying QiSpace QK API.
- #### MASQ: Coming soon. The MASQ API is currently unsupported on iOS devices. If urgent access is necessary, contact Quantropi support directly.

On the landing page, a user can  access additional demo pages that illustrate the SEQUR, QEEP and MASQ (coming soon) APIs via buttons and text boxes. Swiping down from any page allows the user to return back to the landing page.

## **Installation and Preparation**
1. Download this git repository to a local device.
2. Replace the current placeholder `/QiSpace_SDK_Lib` folder with the `/QiSpace_SDK_Lib` containing QEEP and SEQUR libraries.
    * Note: If those libraries and files are given under a folder with a different name, simply copy their contents into the `/QiSpace_SDK_Lib` folder. The same libraries and files are compatible for the Swift and Objective C demos.
3. Copy the `qispace_info.h` file into `/QiSpaceObjectiveC` folder.
4. Open and setup XCode and select iPhone 13 as desired device.
5. Use XCode to add all files obtained at Steps 2 and 3 into the demo projects.

    * Note: If any files/directories mentioned above are not available, please contact Quantropi support to obtain yours.

## **Getting Started**
1. Open this repository within XCode
2. Run the application

## **Core Functionalities**
### SEQUR `Sequr_Demo`:
_Assume the QiSpace device token is already established._
1. SETUP SUBKEY: Establish the subscriber key by interfacing with QiSpace REST API, extract, then convert the result to operable byte array state.
2. GET QE: Obtain the Quantum Entropy from QiSpace REST API, extract, and unpack the data to operable byte array.
3. GENERATE RANDOM STRING: Configure and call PQRND API to generate random string with the QE as seed.

* Note: This demo page only shows an instance of QE decoding, but it can also be applied to QK.

### QEEP `Qeep_Demo`:
_Assume the sub_key and token are already established._
- GET QK: Retrieve, convert and decode the Quantum Key from QiSpace REST API.
- ENCRYPT: Use QEEP API to encrypt the message with the QK.
- DECRYPT: Use QEEP API to decrypt the encrypted message with the QK.

### MASQ `Masq_Demo` (Coming soon):

## **License**
See [LICENSE](LICENSE). Copyright © Quantropi, Inc

Refer to documentation site for more general information about SEQUR, QEEP and PQRND APIs as well as QiSpace REST API.


