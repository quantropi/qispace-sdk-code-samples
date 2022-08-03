# QiSpace-SDK-Demo-Swift-App
A functionality iOS demo app made using Swift and its standard SwiftUI library to demonstrate the functionality and usage of QiSpace QEEP and SEQUR APIs.

Note: This app was developed on an emulated 7th gen iPod Touch. UI elements may differ on devices with different displays. Internet access is required to run this app.

## **Overview**
- #### SEQUR: Demonstrates the functionality of SEQUR API and the QiSpace QE REST API
- #### QEEP: Shows an instance of QEEP API usage and the accompanying QiSpace QK REST API
- #### MASQ: Coming soon. The MASQ API is currently unsupported on iOS devices. If urgent access is necessary, contact Quantropi support directly.

On the landing page, a user can access additional demo pages that illustrate the SEQUR, QEEP and MASQ (coming soon) APIs via buttons and text boxes. The back button may be pressed from any page to return to the landing page.

## **Installation and Preparation**
1. Download this git repository to a local device.
2. Replace the current placeholder `/QiSpace_SDK_Lib` folder with the `/QiSpace_SDK_Lib` containing QEEP and SEQUR libraries.
    * Note: If those libraries and files are given under a folder with a different name, simply copy their contents into the `QiSpace_SDK_Lib` folder. The same libraries and files are compatible for the Swift and Objective C demos
3. Copy the `QiSpace_Info.swift` file into `/QiSpaceiOSDemoApp` folder.
4. Open and setup XCode and select iPod touch (7th gen) as desired device.
5. Use XCode to add all files obtained at Steps 2 and 3 into the demo projects.

    * Note: If any files/directories mentioned above are not available, please contact Quantropi support *

## **Getting Started**
1. Open this downloaded repository within XCode
2. Run the application

## **Core Functionalities**
### SEQUR `SwiftDemo`:
_Assume the API token is already established_
- SETUP SUBKEY: Establish the subscriber key by interfacing with QiSpace REST API and converts the result to operable state
- GET QE: Obtain Quantum Entropy from QiSpace REST API and unpacks the data to operable byte array
- GENERATE RANDOM STRING: Configure and call PQRND API to generate random string with the QE as seed

*Note: This demo page only shows an instance of QE decoding, but it can also be applied to QK *

### QEEP `QeepDemo`:
_Assume the sub_key and token are already established._
- GET QK: Retrieve, convert, and decode Quantum Key from QiSpace REST API
- ENCRYPT: Use QEEP API to encrypt the message with QK
- DECRYPT: Use QEEP API to decrypt the encrypted message with QK

### MASQ `MasqDemo` (Coming soon):

## **License**
See [LICENSE](LICENSE). Copyright © Quantropi, Inc

Refer to documentation site for more general information about SEQUR, QEEP, and PQRND APIs as well as QiSpace REST API.


