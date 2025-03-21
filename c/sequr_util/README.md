# QiSpace Sequr Util Demo for C Application

  A functionality demo based in C to demonstrate the usage of the QEEP, SEQUR, and PQRND APIs as well as QiSpace RESTful API.

  The demos are tested on Ubuntu, MacOS Intel and M1, RaspberryPI.

## **Overview**

- **SEQUR**: Demonstrates the use of **SEQUR API to decode arbitrary QiSpace Enterprise payloads** using either a pre-loaded or QiSpace REST API retrieved subscriber key (subkey).
- **QEEP**: Demonstrates the use of **QEEP API for encryption/decryption**. 
- **PQRND**: Demonstrates the use of **PQRND API for generating Pseudo Quantum Random Data** with either pre-loaded Quantum Entropy (QE) or QiSpace REST API retrieved QE.

## **Installation and Preparation**
1. Download or checkout `qispace-sdk-code-sample` git repository to the local machine.

2. Install demo dependencies: `cjson` and `curl`.
  
    - Ubuntu 22.04 and RaspberryPI:  `sudo apt-get install libcjson-dev libcurl4-openssl-dev`
    - Ubuntu 24.04: `sudo apt-get install libcjson-dev libcurl4-gnutls-dev`
    - MacOS Intel and M1:  `brew install curl cjson`

3. Contact Quantropi to get the latest QiSpace QEEP SDK libraries.

4. Create the `libqeep` folder at the top of this repository.

5. Once the latest QiSpace QEEP SDK libraries has been obtained, `copy & paste` all the contents from `lib` folder into `libqeep` according to the building platform.

6. Get the `qispace_meta.json` config file from Quantropi Inc. If you do not have one, please contact Quantropi Inc.


## **Build Demo**

The demo can be build as per below run command corresponding to each host platform.

1. Ubuntu: 
    
    Library: `libqeep-gcc-x64.a` 

    Run Command: `make`

2. RaspberryPI: 
    
    Library: `libqeep-rpigcc64-aarch64.a`

    Run Command: `make`

3. Mac Intel: 

    Library:`libqeep-clang-x64.a`

    Run Command: `make`

4. Mac M1: 
    
    Library: `libqeep-clang-arm64mac.a`

    Run Command: `make MAC_M1=1`

## **Run Demo**

We have four different demos showing the functionality of QiSpace SDK.

1. **Generate Key demo**

    We support variable length key generation for both classical symmetric ciphers and quantum-safe ciphers such as AES and QEEP.

    - Execute the `demo_sequr_key_gen` with the below arguments:

      ```
      Usage: ./demo_sequr_key_gen [-h] [--qispace_meta QISPACE_META] [--key_size_bits KEY_SIZE_BITS] [--key_type KEY_TYPE]

      options:
        -h, --help        Show this help message and exit
        --qispace_meta    Path to qispace meta .json file, provided by Quantropi Inc.
        --key_size_bits   Key size to generate (in bits, defaults: 256)
        --key_type        0: AES key, 1: QEEP Key, default: AES Key
      ```

    - You will get an output as below:

      ```
      ------------------------
      Key generation successful
      Key ID: c882be4f-a21f-4981-ac8b-d2ca42b035e4
      Key: beaa0fb2bc56a1bc8851a28a2bd53b0ab2f58f7d3eb1d002ff63353855e357fd
      ------------------------
      ```

2. **Query Key demo**

    - Execute the `demo_sequr_key_query` with the below arguments:

      ```
      Usage: ./demo_sequr_key_query [-h] [--qispace_meta QISPACE_META] [--key_id KEY_ID] [--key_type KEY_TYPE]

      options:
        -h, --help           Show this help message and exit
        --qispace_meta       Path to qispace meta .json file, provided by Quantropi Inc.
        --key_id             Key ID to query
        --key_type           0: AES key, 1: QEEP Key, default: AES Key
      ```

    - You will get an output as below:

      ```
      ------------------------
      Key query successful
      Key ID: c882be4f-a21f-4981-ac8b-d2ca42b035e4
      Key: beaa0fb2bc56a1bc8851a28a2bd53b0ab2f58f7d3eb1d002ff63353855e357fd
      ------------------------
      ```

    - Compare the outputs between Generate key and Query key demo

      For the same `key_id`, we were able to retrieve the same key content in both AES key and QEEP key format.


3. **Generate QE demo**

      QE is the Quantum Entropy from QiSpace hosted quantum random source.

      - Execute the `demo_sequr_qe_gen` with the below arguments:

        ```
        Usage: ./demo_sequr_qe_gen [-h] [--qispace_meta QISPACE_META] [--length len]

        options:
          -h, --help           Show this help message and exit
          --qispace_meta       Path to qispace meta .json file, provided by Quantropi Inc.
          --length             Length of required QE in byte
        ```

      - You will get an output as below:

        ```
        ------------------------
        QE query successful
        QE: 783966776b74486a70486e38
        ------------------------
        ```
4. **QEEP demo**

      QEEP demo provides the example code to use QEEP cipher APIs and QEEP Key.

      - Execute the `demo_qeep` with the below arguments:

        ```
        Usage: ./demo_qeep [-h] [-k qeep_key] [-m message]

        options:
          -h, --help        Show this help message and exit
          -k                qeep key hex string\n";
          -m                message string to encrypt\n";
        ```

      - You will get an output as below:

        ```
        ------------------------
        Demo QiSpace QEEP Encode operation...
        Creating QEEP handle...
        Success!
        Loading QEEP key...
        Success!
        Loading message IV...
        Success!
        Encrypting the message...
        Success!
        Demo QiSpace QEEP Encode operation done.

        Demo QiSpace QEEP Decode operation...
        Creating QEEP handle...
        Success!
        Loading QEEP key...
        Success!
        Loading message IV...
        Success!
        Decrypting the encrypted message...
        Success!
        Demo QiSpace QEEP Decode operation done.

        Compare message and decrypted message...
        Match: decrypted_message = message 
        Input message: 
          68656c6f6f6f6f6f
        Encrypted message: 
          33699576372dfe89
        Decrypted message: 
          68656c6f6f6f6f6f
        ------------------------
        ```

5. **PQRND demo**

      PQRND demo provides example code to use PQRND APIs with QE to generate quantum random string.

      - Execute the `demo_pqrnd` with the below arguments:

        ```
        Usage: ./demo_pqrnd [-h] [--qe QE] [--length len]

        options:
          -h, --help           Show this help message and exit
          --qe                 QE hex string to seed PQRND
          --length             Required length of pseudo quantum random string to generate in bytes
        ```

      - You will get an output as below:

        ```
        ------------------------
        Demo SEQUR PQRND operations...
        Initializing PQRND handle...
        Success!
        Loading PQRND seed...
        Success!
        Generate PQRND...
        Success!
        Generated Quantum random data: 72 bb 91 8 ef bc c0 42 5 70 41 fd 7d af 3e 42 45 fa eb 85 7 a6 8 64 9c 31 23 82 4c f4 1f b3 
        Done!
        ------------------------
        ```