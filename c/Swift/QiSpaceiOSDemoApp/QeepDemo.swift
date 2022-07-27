//
//  QeepDemo.swift
//  QEEPiOSDemoApp
//
//  Created by Roy Zheng on 2022-06-01.
//

import SwiftUI

/// This page demonstrates a use of the QEEP API to encrypt and decrypt a message with a  QiSpace retrieved Quantum Key (QK) and iv
/// Additionally, it shows using SEQUR API as a means to SEQUR decode the retrieved and encrypted Quantum Key.
///
///** MUST setup the subscriber key (subkey) from SequrDemo before using QK since QK decryption requires subkey to be initialized**
///
/// Prerequisites:
///  - An enterprise device token which will be used as authentication for QK retrieval from QiSpace REST API
///  - A subkey intended for SEQUR decoding the QK after its retrieval
///
/// Logical flow:
///  1. Press 'Get QK' to obtain QK for encrypting/decrypting purposes
///  2. Enter a message into the text field
///  3. Press 'Encrypt' to QEEP encrypt the text field message
///  4. Press 'Decrypt' to QEEP decrypt the previously encrypted message

struct QeepDemo: View {
    
    let qeep_demo_BG_colour = Color(red: 0, green: 0, blue: 0, opacity: 77.5/100)
    
    // Attributes for modifiable UI texts
    @State var user_message: String = "Enter message"
    @State var encrypted_message_string : String = "encrypted message"
    @State var decrypted_message_string : String = "decrypted message"
    
    // Attributes for API-related operations
    @State var encrypted_message_bytes: [UInt8]?
    @State var decrypted_message_bytes: [UInt8]?
    @State var message_iv: [UInt8]? = nil
    let qk_endpoint = qispace_info.base_url + "/api/v1/qk/"
    @Binding var sub_key : [UInt8]
    @State var qk : [UInt8]? = nil
    
    // Note: Token is set as this for this demo. Normally, token is generated upon device setup
    // Actual device token was removed for privacy reasons
    let token = "Bearer " + qispace_info.access_token
    
    var sequr_wrapper : sequrSwiftWrapper? = sequrSwiftWrapper()
    var qeep_wrapper : qeepSwiftWrapper? = qeepSwiftWrapper()
    
    /// Stores the retrieved Quantum Key (QK) from QiSpace Enterprise into subkey global variable
    ///
    /// - Prerequisites:
    ///  - Valid device token: a valid enterprise device token string needed to retrieve QK from QiSpace Enterprise
    ///  - subkey: a byte array used to retrieve QK from QiSpace Enterprise (need to setup subkey in SEQUR before getting QK)
    /// - Parameters: None
    /// - Returns: None
    func get_qk(){
        let sequr_wrapper = sequrSwiftWrapper()
        
        // Sets up HTTP request objects with specifications
        var qk_request = URLRequest(url: URL(string: qk_endpoint)!)
        qk_request.httpMethod = "POST"
        let json : [String: Any] = ["key_length" : 32, "label": "demo-device", "isExternal" : true]     // Note: In this demo, key length is set to be 32 bytes
        let bodyJSON = try? JSONSerialization.data(withJSONObject: json)
        qk_request.httpBody = bodyJSON
        qk_request.setValue("application/json", forHTTPHeaderField: "Content-Type")     // Note: Content-Type must be of application/json
        qk_request.setValue(token, forHTTPHeaderField: "Authorization")
        

        /// QiSpace API response consists of
        /// a payload, a pre-encrypted qeep key (QK),
        /// and an iv, a variable that diminishes data enryption repetition,
        /// all in Base64 format
        let qk_operation = URLSession.shared.dataTask(with: qk_request, completionHandler: { data, response, error -> Void in
            
            // Check for API request error and display
            if let error = error{
                print("Request error: ", error)
                return
            }
            
            guard let response = response as? HTTPURLResponse else { return }
            if response.statusCode == 200{      // Note: Currently, only 200 is thrown on success. Might need to configure checks for different success codes in the future
                DispatchQueue.main.async{
                    do{
                        let qk_payload = try JSONSerialization.jsonObject(with: data!) as? [String: Any]
                        
                        // Convert the payload and iv from Base64 to byte array
                        var encrypted_QK : [UInt8] = SequrDemo.base64_to_bytes(
                            string: String(describing: qk_payload!["payload"]!)
                        )
                        var qk_iv : [UInt8] = SequrDemo.base64_to_bytes(
                            string: String(describing: qk_payload!["iv"]!)
                        )
                        
                        // SEQUR decode the payload to get  Qeep Key (QK) with sub_key
                        self.qk = [UInt8].init(repeating: 0, count: encrypted_QK.count)
                        var ret = sequr_wrapper.decode(
                            key: &self.sub_key,
                            iv: &qk_iv,
                            encoded_message: &encrypted_QK,
                            decoded_message: &self.qk!
                        )
                        
                    }catch{
                        print("error with qe/iv")
                    }
                }
            }
        })
        
        // Trigger QK retrieval from QiSpace REST API
        qk_operation.resume()
    }
    
    
    /// 'Encrypt' button press event
    func encrypt_btn_pressed(){
        var message_bytes: [UInt8] = Array(user_message.utf8)
        encrypted_message_string = encrypt_operation(key: &(qk), iv: &message_iv, message: &message_bytes, encrypted_message: &encrypted_message_bytes)
    }
    
    
    /// Performs encryption using the QEEP algorithm on a passed-in byte array message
    ///
    /// - Prerequisites:
    ///  - QK: Must be retrieved before attempting encryption/decryption
    ///  - iv:  must be setup in order to use during encryption process
    /// - Parameters:
    ///  - key: a cryptographic key used to perform the encryption
    ///  - iv: initialization vector to improve randomness
    ///  - message: the message to be encrypted as a byte array
    ///  - encrypted_message: the message after encryption
    /// - Returns: A QEEP encrypted form of the message or an error statement
    func encrypt_operation(key qk: inout [UInt8]?, iv message_iv: inout [UInt8]?, message message_bytes:inout [UInt8], encrypted_message encrypted: inout [UInt8]?) -> String{
        
        // Verify that QK is present before attempting to encrypt/decrypt
        if(qk == nil){
            return "Need to retrieve QK before encrypting"
        }
        
        // Initialize data
        encrypted = [UInt8].init(repeating: 0, count: message_bytes.count)
        decrypted_message_bytes = [UInt8].init(repeating: 0, count: message_bytes.count)
        message_iv = SequrDemo.hex_string_to_bytes("01020304050607080901020304050607")!        // Note: Generates random 16 bytes iv per encrypt operation or per message. Currently fixed for this demo
        
        // Encrypts message with QEEP wrapper class
        let ret: Int32 = qeep_wrapper!.encrypt(
            key: &(qk!),
            iv: &message_iv!,
            message: &message_bytes,
            encrypted_message: &encrypted!
        )
        
        // Handle output based on encrypt result
        if (ret == 1){
            // Converts byte array into hex string for printing
            return String(Data(encrypted!).hexEncodedString())
        }else{
            return "Error code: \(ret) occurred in encryption. Consult error.h for more information"
        }
    }
    
    /// 'Decrypt' button press event
    func decrypt_btn_pressed(){
        // Perform the decryption
        decrypted_message_string = decrypt_operation(key: &(qk), iv: &message_iv, encrypted_message: &encrypted_message_bytes, message: &decrypted_message_bytes)
    }
    
    /// Performs decryption using the QEEP algorithm on a passed-in encrypted byte array
    ///
    ///  - Prerequisites:
    ///   - key (QK): Must be retrieved before attempting encryption/decryption
    ///   - iv: Must be setup in order to use during encryption process
    ///  - Parameters:
    ///    - key (QK): the same cryptographic key used to perform the decryption
    ///    - iv: initialization vector to improve randomness
    ///    - encrypted_message: the encrypted message as a result of pressing the encrypt button
    ///    - message: the plaintext message that results from QEEP decrypt
    ///  - Returns: A QEEP decrypted form of the encrypted message or the issue being faced
    func decrypt_operation(key qk:inout [UInt8]?, iv message_iv: inout[UInt8]?, encrypted_message encrypted_message_bytes: inout [UInt8]?, message decrypted_message_bytes: inout [UInt8]?) -> String{
        // Verifies that QK is present before attempting to encrypt/decrypt
        if(qk == nil){
            return "Need to retrieve QK before decrypting"
        }
        if(encrypted_message_bytes == nil){
            return "Need to encrypt before decrypting"
        }
        
        // Decrypts the encrypted message
        // Note: Message iv should be identical to one used during encryption
        let ret: Int32 = qeep_wrapper!.decrypt(
            key: &qk!,
            iv: &message_iv!,
            encrypted_message: &encrypted_message_bytes!,
            message: &decrypted_message_bytes!
        )

        // Handles UI display according to error codes
        if (ret == 1){
            // Outputs the byte array as string to screen
            return String(bytes: decrypted_message_bytes!, encoding: .utf8)!
        }else{
            return "Error code: \(ret) occurred in encryption. Consult error.h for more information"
        }
    }
    
    @Environment(\.presentationMode) var presentationMode: Binding<PresentationMode>
    var btn_back : some View {
        Button(action:{
            self.presentationMode.wrappedValue.dismiss()
        }) {
            HStack{
                Image("back_button_image")
                    .aspectRatio(contentMode: .fit)
                    .foregroundColor(Color.white)
                Text("Back")
            }
        }
    }
    
    var body: some View {
        
        ZStack{
            qeep_demo_BG_colour
                .ignoresSafeArea()
            VStack{
                HStack{
                    Spacer().frame(width: 10)
                    
                    Button(action: get_qk){
                        Text("GET QK")
                            .padding()
                            .background(Color.gray)
                            .foregroundColor(.white)
                    }
                    Spacer()
                }
                Spacer()
                HStack{
                    VStack{
                        TextField("Enter message", text: $user_message)
                            .keyboardType(.default)
                            .padding(.horizontal, 30).padding(.top, 20)
                            .multilineTextAlignment(TextAlignment.center)
                            .foregroundColor(Color.white)
                        
                        Divider()
                            .padding(.horizontal, 30)
                            .background(Color.white)
                    }
                    
                    Button(action: encrypt_btn_pressed){
                        Text("ENCRYPT")
                            .padding()
                            .background(Color.gray)
                            .foregroundColor(.white)
                    }
                }
                Spacer()
                
                HStack{
                    Text("Encrypted Message:")
                    Text(encrypted_message_string)
                }.foregroundColor(Color.white)
                
                Spacer()
                
                Button(action: decrypt_btn_pressed){
                    Text("DECRYPT")
                        .padding()
                        .background(Color.gray)
                        .foregroundColor(.white)
                }
                
                Spacer()
                
                HStack{
                    Text("Decrypted Message: ")
                    Text(decrypted_message_string)
                }.foregroundColor(Color.white)
                Spacer()
               
            }.padding()
        }
        .navigationBarBackButtonHidden(true)
        .navigationBarItems(leading: btn_back)
    }
}

//struct QeepDemo_Previews: PreviewProvider {
//    static var previews: some View {
//        let requests = Requests()
//        QeepDemo().environmentObject(requests)
//    }
//}
