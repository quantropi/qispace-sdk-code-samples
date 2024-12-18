# IoT Renesas Demos

This README explains how to run the QiSpace Demo on two Renesas boards: `CK-RA6M5` and `EK-RA6M5`

To confirm the QiSpace SDK Release version please check [SDK_Version.txt](SDK_Version.txt).

# CK-RA6M5
The `qispace_ck_ra6m5` project under the `CK-RA6M5` folder will be used to demo how to use QiSpace Products: MASQ DS, MASQ KEM

## Device

Renesas CK-RA6M5 v1

## Environment

e2studio (Version: 2024-01.1, 24.1.1, FSP: 5.2 ) on Windows.

## Build
1. Launch e2studio with FSP 5.2.
2. Import `qispace_ck_ra6m5` to the workspace.
3. Check the libraries from latest release are available under `qispace-SDK-lib` folder under `qispace_ck_ra6m5` folder.
4. Build the project and then download the binary on the board using Run/Debug.

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
```

If all the demos ran successfully, the LEDs will start blinking.

# EK-RA6M5

`EK-RA6M5` has two demos inside:

- `qispace_ek_ra6m5_ds`: MASQ DS demo

- `qispace_ek_ra6m5_kem`: MASQ Kem demo

## Device
Renesas EK-RA6M5

## Environment
e2studio (Version: 2024-01.1, 24.1.1, FSP: 5.2 ) on Windows.

## Build and Run
1. Launch e2studio with FSP 5.2.
2. Import `qispace_ek_ra6m5_ds` and `qispace_ek_ra6m5_kem` to the workspace.
3. Check the libraries from latest release are available under `qispace-SDK-lib` folder under `qispace_ek_ra6m5_ds` and `qispace_ek_ra6m5_kem` folder.
4. Build each of the project separately and download only one binary at a time on the board using Run/Debug.
5. Use a serial terminal application like `Tera Term` on Windows to do the communication over USB. After the binary is launched to the board, the `Tera Term` console will be active, press `return` button to run any demo.