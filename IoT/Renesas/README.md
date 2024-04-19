The qipace_m5 project in the ck-ra6m5_LED_Blink folder will be used to demo how to use our QiSpace: DS, KEM, QEEP and SEQUR 

# Device
    Renesas CK-RA6M5
     
# Enviroment
    e2-studio (Version: 2024-01.1, 24.1.1, FSP: 5.2 ) on Windows.

# Feature
     This c/c++ project was created with "new project wizard" for CK-RA6M5, GNU ARM toolchain, Non-TrustZone and using "Bare-metal Blinky" template.

     Following code is added to the hal_entry.c
        ret = maintest();
        if (ret != 0) {
                return;
        }

     The maintest() will call following demo:
        ret1 = ds_demo();
        ret2 = kem_demo();
        ret3 = qeep_demo();
        ret4 = sequr_demo();

     The source code of those 4 demos will show how to use DS, KEM QEEP and SEQUR.

     If all thoes demos worked fine, the LEDs will be blinking.

# Build
     1. Launch e2studio with FSP 5.2
     2. Import qispace_m5 to workspace
     3. Copy follwing release folders under qispace_m5 folder
                qispace-MASQ-SDK-R1.8.2-iot
                qispace-QEEP-SDK-R1.8.2-iot
     4. Build and run