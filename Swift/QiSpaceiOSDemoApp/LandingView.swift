//
//  fakeContentView.swift
//  QEEPiOSDemoApp
//
//  Created by Roy Zheng on 2022-05-31.
//

import SwiftUI

/// Redirects user to each of the following demos to explore functionality for SEQUR, QEEP:
///  - SequrDemo: Demonstrates subkey setup process (which will usually be done on device setup), QE retrieval and unpacking with SEQUR API, and PQRND API pseudo quantum random data generation
///  - QeepDemo: Demonstrates QK retrieval from QiSpace Enterprise, decoding QK with SEQUR API and encryption/decryption with QEEP API
/// Note: Setup subkey in SequrDemo should be executed before QeepDemo since subkey is initialized with an outdated subkey
struct LandingView: View {

    /// Returns the byte array equivalent of a hex string
    static func hex_string_to_bytes(_ string: String) -> [UInt8]? {
        let length = string.count
        if length & 1 != 0 {
            return nil
        }
        var bytes = [UInt8]()
        bytes.reserveCapacity(length/2)
        var index = string.startIndex
        for _ in 0..<length/2 {
            let nextIndex = string.index(index, offsetBy: 2)
            if let b = UInt8(string[index..<nextIndex], radix: 16) {
                bytes.append(b)
            } else {
                return nil
            }
            index = nextIndex
        }
        return bytes
    }
    
    // Currently contains an outdated subkey as placeholder text.
    // When running app, MUST setup subkey first in the SequrDemo page
    @State private var sub_key: [UInt8] = hex_string_to_bytes("01dbd52abda3babbb1d0613c86d96968fe552a05a66364e733f499e1a7c827e5d24bde8b56bb6a8fda54798e7c0366a8beb6401c180c05a3741458e521da8cdc79989018d7313b940164067f145c419287f398658814cabd0ba1427ef0a8cd2b8e7349d486dc18f4daba291a9329e93b755753cd0a4a9b4affaa3f42724fa07f1437fe6937fd5a997dc60889e331c63c0e63d192b5f5b298a2239863a6b005d445f4d63121ae93ac15782aa3159afb12401e98ede1689a2294b4bccf06cbd4466775fc51a2495a6f5e8c5a3839bd8995d04b22ea6be1200b0253117c4758371440cab73f83794b536a27c31c76bb18362222fe5b1e3c92837ce5cc45ffb14b0b000deccf1bf3d58de0f4b7416c896231894a10792f4d075bbd5a5d6c095ecb8d87b765074f6fd354cde531c4d0a69cab4a15f106067d27f9874751e9738a450cf26a943d46436493ad06e9a407ecd3a48fa327f9df1f9b4b20a9af6d5f35b6ed0fd2d473d250651ab03766ead4e268aa63b520351c9f65068a0f79984809e7026e3923f4b1883ee2e4b37b4452d62a5898528d82f54fb02e184a8350161c68a81c92068fd72371c32538de6ccc73146ccc89294e4d48e9476ec1ddedac4e41dccdc0a21ca585c2a439452462fe9e13245bf7207699f03180dd214f8f6e597612a144e2237bc8dd40e136b916d10cfdc7b4b9d8f375c7c4c972c1bb4c14c4c5054d30b70488dd91658017f65cdfa354679ce602263a2fe93b4c88f2ef9a1c7dbc8e19924025b886b087b418845cb423eebbaefd33dcf3d814f0f3cd43ab1a9d91658bd7b462805bc3419ee4b99ba632c80f49317f738706641b3a655a9024eee27be6eb99b3e1b419a4f41221de5b9d7c72472400594ad9b5e6c7c9b865cb9c2cec437cc463ac8fed49c02fded90bebe4bab16e667d97c2ea699a40f6eadc6324609fcf8d81cb318aa55d265fb0113c667f87bd865fe3f7811443fdc361550e6869b641d4ebf71611dd8e713a83014fa0c71076939a08a5a791abd1f48908ca792c7af7e2451eaa08714d090ad6809f5583c6a46427e1d25c394c6b780b53905f6ddc912223f71914bd8c15d8d659152577174fcb44c69023f32c8f96bfcb9b9aab5d7256f5a71b26cd64b16c0407d2e02a5ef97d6d9032376d2fea871b170eb67838666f254ecb005010fef0206b3cafcdf58762efed6fd833f8804b2cd632f6071564a3cc66103cb58fb7761f36d7c81980a5706924ce7cfa64db738faf70016eb4382cafaeee06b2cf18e7718ed9560a4e6f572c9fb0c38e24d19b47100008a4a0e068716aca511bf8a3f8cfb680212f442d04764ebfe195a52358a688dfb867262b7c3a52c7efe5a74addd9a06ad895c41a2de0ffd49b04a2f0224e0cc5653cc0f54bd8655d830b432c8c6cfa87673f74c00ff2a195614f99d4a09611cf1aa36a8cfcf559892be4f99a8e2022e4012339977a25ff")!
    
    let landing_page_BG = Color(red: 0, green: 0, blue: 0, opacity: 77.5/100)
    
    var body: some View {

            NavigationView{
                ZStack{
                    landing_page_BG
                    VStack{
                        Text("QiSpace Demo App - Swift")
                            .ignoresSafeArea()
                            .padding(.bottom)
                            .font(.largeTitle)
                            .multilineTextAlignment(.center)
                        
                        Spacer().frame(height: 33)
                        
                        NavigationLink(destination: SequrDemo(sub_key: $sub_key)){
                            Text("Sequr Demo")
                                .padding()
                                .background(Color.gray)
                                .foregroundColor(.white)
                        }
                        
                        Spacer().frame(height: 25)
                        
                        NavigationLink(destination: QeepDemo(sub_key: $sub_key)){
                            Text("Qeep Demo")
                                .padding()
                                .background(Color.gray)
                                .foregroundColor(.white)
                        }
                        
                        Spacer().frame(height: 25)
                        
                        NavigationLink(destination: MasqDemo()){
                            Text("Masq Demo")
                                .padding()
                                .background(Color.gray)
                                .foregroundColor(.white)
                        }
                    }
                }
                .ignoresSafeArea()
                .frame(maxWidth: .infinity, maxHeight: .infinity)
                
            }
    }
}


struct fakeContentView_Previews: PreviewProvider {
    static var previews: some View {
        LandingView()
    }
}
