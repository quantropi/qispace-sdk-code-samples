# QiSpace Python Demos

## Sequr

### Setup
  1. Place your QEEP `.whl` library file under `lib` folder. If you do not have one, please contact Quantropi Inc.
  1. Install demo dependencies and libraries: `./install.sh`
  1. Have your QiSpace meta config `.json` file from Quantropi Inc ready. If you do not have one, please contact Quantropi Inc.

### Running the demo

  1. **Generate key demo:**
      - Execute the `demo_sequr_key_gen.py` with the below arguments:
        ```
        usage: ./demo_sequr_key_gen.py [-h] --qispace_meta QISPACE_META [--key_size_bits KEY_SIZE_BITS]

        options:
          -h, --help            show this help message and exit
          --qispace_meta QISPACE_META
                                Path to qispace meta .json file, provided by Quantropi Inc
          --key_size_bits KEY_SIZE_BITS
                                Key size to generate (in bits, defaults: 256)
        ```
      - You might get an output as below:
        ```
        ------------------------
        Key generation successful
        Key ID: 60d27860-3be1-4eae-92a3-2c10129eeb60
        Key: 86 83 b1 57 a2  ...  b7 90 83 65 fe
        ------------------------
        ```
  1. **Query key demo:**
      - Execute the `demo_sequr_key_query.py` with the below arguments:
        ```
        usage: ./demo_sequr_key_gen.py [-h] --qispace_meta QISPACE_META --key_id KEY_ID

        options:
          -h, --help            show this help message and exit
          --qispace_meta QISPACE_META
                                Path to qispace meta .json file, provided by Quantropi Inc
          --key_id KEY_ID       Key ID to query
        ```
      - You might get an output as below:
        ```
        ------------------------
        Key query successful
        Key: 86 83 b1 57 a2  ...  b7 90 83 65 fe
        ------------------------
        ```
  1. **Compare results**

      Note that for the same `key_id`, we were able to retrieve the same key content.

### Implementation Note
A convenient `SequrUtil` class has been implemented in `QiSpaceSdkLib.py`. This class implements common encryption and decryption logic that can be used in any end-application.
