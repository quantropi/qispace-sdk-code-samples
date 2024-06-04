This README explains how to run the QiSpace Demo on two Renesas boards.

# qispace_m5
The `qispace_m5` project under the `ck-ra6m5_LED_Blink` folder will be used to demo how to use QiSpace Products: DS, KEM, QEEP and SEQUR 

## Device

Renesas CK-RA6M5 v1
     
## Enviroment

e2-studio (Version: 2024-01.1, 24.1.1, FSP: 5.2 ) on Windows.

## Feature
This c/c++ project was created with `New Project Wizard` for `CK-RA6M5`, `GNU ARM toolchain`, `Non-TrustZone` and using `Bare-metal Blinky` template.

Following code is added to the `hal_entry.c`:
```
ret = maintest();
if (ret != 0) {
        return;
}
```

The `maintest()` will call the following demo:
```
ret1 = ds_demo();
ret2 = kem_demo();
ret3 = qeep_demo();
ret4 = sequr_demo();
```

If all those demos worked fine, the LEDs would be blinking.

## Build
1. Launch e2studio with FSP 5.2
2. Import `qispace_m5` to the workspace
3. Copy the following release folders under `qispace_m5` folder:

    qispace-MASQ-SDK-R1.8.2-iot

    qispace-QEEP-SDK-R1.8.2-iot

4. Build and run


# qispace_ek-ra6m5
The `qispace_ek-ra6m5` project under the `ek-ra6m5_Console` folder will demo how to use QiSpace Products: DS, KEM, QEEP and SEQUR.

The Serial Console will display the below menu options to run the different demos:
```
Welcome to QiSpace Demo for EK-RA6M5!  Press 
1 for KEM demo
2 for DS demo
3 for QEEP demo
4 for SEQUR demo
```

## Device
Renesas EK-RA6M5
   
## Enviroment
e2-studio (Version: 2024-01.1, 24.1.1, FSP: 5.2 ) on Windows.

## Configuration
```
Main stack size(bytes): 0x1000
Heap size(bytes): 0xD000
```

## Feature
Use a serial terminal application like `Tera Term` on Windows to do the communication over USB.

After the binary is launched to the board, the `Tera Term` console will be active, press `return` button and the above menu will show up on the terminal.

Press 1 to run the KEM demo, Press 2, 3 or 4 to run other demos.

## Build
1. Launch e2studio with FSP 5.2
2. Import `qispace_ek-ra6m5` to the workspace
3. Copy the following release folders under `qispace_ek-ra6m5` folder:

    qispace-MASQ-SDK-R1.8.2-iot

    qispace-QEEP-SDK-R1.8.2-iot

4. Build and run
