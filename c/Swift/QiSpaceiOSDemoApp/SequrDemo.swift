//
//  SequrDemo.swift
//  QEEPiOSDemoApp
//
//  Created by Roy Zheng on 2022-06-01.
//

import SwiftUI

/// To decode any payload from QiSpace RESTful API, we use SEQUR decode().
/// This demo shows  SEQUR decoding applied to QE (Quantum Entropy) 
/// Prerequisite: A device token for QiSpace API call

/// Note: Only one subkey and one active token per enterprise device

/// Logical flow:
///  1. Press 'Setup subkey' to establish the sub_key between client and QiSpace Enterprise
///  2. Press 'Get QE' to get (and decode) the Quantum Entropy (QE) from QiSpace Enterprise
///  3. Press 'Get PQRND' to get the pQrnd string (by seeding it with the retrieved QE)

// Returns the hex string equivalent of a byte array
extension Data{
    func hexEncodedString() -> String{
        return map { String(format: "%02hhx", $0) }.joined()
    }
}

struct SequrDemo: View {
    let BGColor : Color = Color(red: 0/255, green: 0/255, blue: 0/255, opacity: 77.5/100)
    
    // QiSpace Endpoints:
    let sub_key_endpoint = qispace_info.base_url + "/api/v1/sub_key"
    let qe_endpoint = qispace_info.base_url + "/api/v1/qe/50"    // Note: QE size is specified to be 50 bytes for this demo
    
    // Attributes related for API
    @Binding var sub_key : [UInt8]
    @State var qe : [UInt8]? = nil
    @State var decoded_qe : String = ""
    @State var myHandle : pQrnd_Handle? = nil
    
    // Note: Token is set as this for this demo. Normally, token is generated upon device setup
    let token = "Bearer "+qispace_info.access_token
    
    // Define attributes for UI display
    @State var decode_string : String = "*some QE value*"
    @State var random_generated_string : String = "*a randomly generated string*"
    @State var random_str : [UInt8]? = [UInt8].init(repeating: 0, count: 32)
    
    var sequr_wrapper : sequrSwiftWrapper? = sequrSwiftWrapper()
    
    
    /// Retrieves, unpacks and stores the subkey from QiSpace Enterprise using Enterprise device's token
    ///
    /// - Prerequisite: Device token for QiSpace API authorization
    /// - Parameters: None
    /// - Returns: None
    func get_subkey(){
        //Setup HTTP request with all relevant data
        var sub_key_request = URLRequest(url: URL(string: sub_key_endpoint)!)
        sub_key_request.httpMethod = "POST"
        sub_key_request.httpBody = try? JSONSerialization.data(withJSONObject: [], options: [])
        sub_key_request.addValue("application/json", forHTTPHeaderField: "Content-Type")        // Note: Content-Type must be of application/json
        sub_key_request.addValue(token, forHTTPHeaderField: "Authorization")
        
        // Sends a POST request to obtain the sub_key, a necessary component to decrypting the payload, in the form of a hex string
        let sub_key_operation = URLSession.shared.dataTask(with: sub_key_request, completionHandler: { data, response, error -> Void in
            if let error = error{
                print("Request error: ", error)
                return
            }
            
            // After successful QiSpace response
            guard let response = response as? HTTPURLResponse else { return }
            if response.statusCode == 200 {
                DispatchQueue.main.async{
                    do{
                        // Process as json as dictionary and converts sub_key from hex string (raw API payload format) to byte array
                        let sub_key_payload = try JSONSerialization.jsonObject(with: data!) as! Dictionary<String, String>
                        self.sub_key = SequrDemo.hex_string_to_bytes(
                            sub_key_payload["sub_key"]!
                        )!
                    } catch {
                        print("error retrieving sub key")
                    }
                }
            }
        })
        
        // Triggers sub_key retrieval from QiSpace REST API
        sub_key_operation.resume()
    }
    
    
    
    /// Retrieves, unpacks and stores the Quantum Entropy (QE) from QiSpace Enterprise using device token and subkey
    ///
    /// - Prerequisites:
    ///  - Enterprise device authorization token: Needed for QE retrieval
    ///  - Sub_key: Required for decoding the encrypted QE
    /// - Parameters: None
    /// - Returns: None
    func get_qe(){
        print("Clicked get_qe")
        let sequr_wrapper = sequrSwiftWrapper()
        
        // Initializes HTTP Request object and provide specifications
        var qe_request = URLRequest(url: URL(string: qe_endpoint)!)
        qe_request.httpMethod = "GET"
        qe_request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        qe_request.setValue(token, forHTTPHeaderField: "Authorization")
        
        // Sends a GET request to retrieve Quantum Entropy (QE) which is later used to seed PQRND API and its IV from QiSpace Enterprise
        let qe_operation = URLSession.shared.dataTask(with: qe_request, completionHandler: { data, response, error -> Void in
            // Checks for QiSpace API request error and display
            if let error = error{
                print("Request error: ", error)
                return
            }
            
            // After successful QiSpace response
            guard let response = response as? HTTPURLResponse else { return }
            if response.statusCode == 200{
                DispatchQueue.main.async{
                    do{
                        let qe_payload = try JSONSerialization.jsonObject(with: data!) as? [String: Any]
                        
                        // Converts QE and respective IV from Base64 to byte array
                        var encrypted_QE : [UInt8]? = SequrDemo.base64_to_bytes(
                            string: String(describing: qe_payload!["payload"]!)
                        )
                        var qe_iv : [UInt8] = SequrDemo.base64_to_bytes(
                            string: String(describing: qe_payload!["iv"]!)
                        )
                        // Performs SEQUR decoding on QE using sub_key as QE is still encrypted
                        self.qe = [UInt8].init(repeating: 0, count: encrypted_QE!.count)        // Note: decrypted and encrypted byte array must be same length for decoding to work. Both are 50 bytes in this example
                        var ret = sequr_wrapper.decode(
                            key: &(self.sub_key),
                            iv: &qe_iv,
                            encoded_message: &encrypted_QE!,
                            decoded_message: &self.qe!
                        )
                        
                        // Formats the string for output
                        self.decoded_qe = String(Data(self.qe!).hexEncodedString())
                        
                        // Resets the PQRND handle
                        self.myHandle = nil
                    }catch{
                        print("error with qe/iv")
                    }
                }
            }
            
        })
        // Trigger QE retrieval request from the QiSpace Enterprise REST  API
        qe_operation.resume()
    }
    
    /// 'Get PQRND' button press event
    func pQrnd_btn_pressed(){
        // Displays output of getting PQRND
        random_generated_string = get_pQrnd(random_seed: &qe!, pqrnd_handle: &myHandle, rand_data: &random_str!)
    }
    
    
    /// Prepares and outputs the pQrnd to screen
    ///
    /// - Prerequisites;
    ///  - Quantum Entropy: Required to be used as seed during pQrnd setup (if first time)
    /// - Parameters:
    ///  - random_seed: a byte array to be used as a seed value (For this demo, QE is passed)
    ///  - pqrnd_handle: a pqrnd handle object for setting up pQrnd API
    ///  - rand_data: the future pseudo random quantum data
    /// - Returns: hex-encoded representation of PQRND data
    func get_pQrnd(random_seed seed: inout [UInt8], pqrnd_handle handle: inout pQrnd_Handle?, rand_data pqrnd: inout [UInt8]) -> String {
        
        // Step 1: Check QE has been retrieved prior
        if(qe == nil){
            return "Need to retrieve QE before generating random string"
        }

        // Seeds pQrnd for the new QE to generate a random value
        var ret : QEEP_RET
        if(myHandle == nil){
            ret = (sequr_wrapper?.pQrnd_setup(seed: &qe!, handle: &myHandle))!
        }
        
        // Generates random number per button press and display
        ret = (sequr_wrapper?.pQrnd_generate(handle: myHandle!, random_number_to_be: &random_str!))!
        
        return String(Data(random_str!).hexEncodedString())
        
        // Note: Remember to clean up pqrnd handle. This is done on back button press
    }
    
    @Environment(\.presentationMode) var presentationMode: Binding<PresentationMode>
    
    var btn_back : some View {
        Button(action:{
            // Closes the pQrnd handle if used
            if myHandle != nil{
                let ret = sequr_wrapper?.pQrnd_close_handle(handle: myHandle!)
            }
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
            BGColor.ignoresSafeArea()
            VStack{
                Spacer()
                
                Button(action: get_subkey){
                    Text("SETUP SUB_KEY")
                        .padding()
                        .background(Color.gray)
                        .foregroundColor(.white)
                }
                
                Spacer()
                
                Button(action: get_qe){
                    Text("GET QE")
                        .padding()
                        .background(Color.gray)
                        .foregroundColor(.white)
                }
                
                VStack{
                    Text("Decoded QE: ")
                    Text(decoded_qe)
                        .font(.caption)
                        .padding()
                }.foregroundColor(Color.white)
                
                
                Spacer()
                
                HStack{
                    Spacer()
                        
                    Button(action: pQrnd_btn_pressed){
                        Text("GET PQRND")
                            .padding()
                            .background(Color.gray)
                            .foregroundColor(.white)
                    }
                    
                    VStack{
                        Text("pQrnd:")
                        Text(random_generated_string)
                            .font(.caption)
                            .padding(.top)
                    }.foregroundColor(Color.white)
                    
                    Spacer()
                }
                Spacer()
            }
        }
            .navigationBarBackButtonHidden(true)
            .navigationBarItems(leading: btn_back)
    }
    
    
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
    
    /// Returns the byte array equivalent of a base64 string
    static func base64_to_bytes(string: String) -> [UInt8]{
        /// Convert base64 -> nsdata -> byte array
        let in_nsdata = Data(base64Encoded: string, options: .ignoreUnknownCharacters)
        let byte_arr = [UInt8] (in_nsdata!)
        return byte_arr
    }
}

//struct SequrDemo_Previews: PreviewProvider {
//    static var previews: some View {
//        let requests = Requests()
//        SequrDemo().environmentObject(requests)
//    }
//}
