//
//  MasqDemo.swift
//  QEEPiOSDemoApp
//
//  Created by Roy Zheng on 2022-06-01.
//

import SwiftUI

struct MasqDemo: View {
    
    let masq_demo_BG_colour = Color(red: 0, green: 0, blue: 0, opacity: 77.5/100)

    var body: some View {
        ZStack{
            masq_demo_BG_colour
                .ignoresSafeArea()
            
            NavigationView{
                Text("Masq is currently unsupported on iOS devices. If this is needed immediately, contact us directly")
                    .multilineTextAlignment(.center)
                    .padding()
                    .font(.title2)
            }
        }
        
        
    }
}

struct MasqDemo_Previews: PreviewProvider {
    static var previews: some View {
        MasqDemo()
    }
}
