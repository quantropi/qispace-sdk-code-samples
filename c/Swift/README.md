# Qeep-SDK-Demo-Swift-APP

An iOS demo app made using Swift and its standard SwiftUI library to demonstrate the functionality and usage of QiSpace QEEP and SEQUR APIs.
Note: The app was developed on an emulated iPod touch (7th gen) so UI elements may be inaccurate on devices with different displays. Furthermore, internet access is required to run this app.

## **Overview:**
- #### SEQUR: Demonstrates the functionality of SEQUR API and the QiSpace QE REST API
- #### QEEP: Shows an instance of QEEP API usage and the accompanying QiSpace QK REST API
- #### MASQ: The MASQ API is currently unsupported on iOS devices and will be coming soon. If urgent access is necessary, contact Quantropi support directly.

Starting on the landing page, the user can then access additonal demo pages that illustrate the SEQUR, QEEP and eventually MASQ APIs via buttons and text boxes. To navigate to another page, the back button may be pressed to return to the landing page.

## **Installation and Preparation:**
1. Download this git repository to a local device
2. Replace the current placeholder `/QiSpace_SDK_Lib` folder with the `/QiSpace_SDK_Lib` containing QEEP and SEQUR libraries
    * Note: If those libraries and files are given under a folder with a different name, simply copy their contents into the `QiSpace_SDK_Lib` folder. The same libraries and files are compatible for the Swift and Objective C demos
3. Copy the `QiSpace_Info.swift` file into `/QiSpaceiOSDemoApp` folder
4. Open and setup XCode and select iPod touch (7th gen) as desired device
5. Make sure to use Xcode add all files obtained at step 2 and 3 into the demo projects.

    * Note: If any files/directories mentioned above are not available, please contact Quantropi support to obtain yours. *

## **Getting Started:**
1. Open this downloaded repository within XCode
2. Run the application

## **Core Functionalities of this Demo:**
### SEQUR `SwiftDemo`:
_Assume the API token is already established_
- SETUP SUBKEY: establish the subscriber key by interfacing with QiSpace REST API and converts the result to operable state
- GET QE: obtain the Quantum Entropy from QiSpace REST API and unpacks the data to operable byte array
- GENERATE RANDOM STRING: configure and call Pseudo Quantum RaNdom Data API to generate random string with the QE as seed
*Note: This demo page only shows an instance of QE decoding, but it can be applied to QK as well

### QEEP `QeepDemo`:
_Assume the sub_key and token are already established_
- GET QK: retrieve, convert and decode the Quantum Key from QiSpace REST API
- ENCRYPT: Use QEEP API to encrypt the message with the QK
- DECRYPT: Use QEEP API to decrypt the encrypted message with the QK

### MASQ `MasqDemo` (Coming soon):

## **License:**
See the [LICENSE] (LICENSE) file. Copyright © Quantropi, Inc

For more information regarding the QiSpace APIs or the QEEP/SEQUR/pQrnd APIs, see official Quantropi documentation


