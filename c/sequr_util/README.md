# QiSpace C Demos

## Sequr

### Setup
  1. Please copy & paste the `lib` folder from QEEP SDK into `libqeep` under the current directory. If you do not have one, please contact Quantropi.
  2. Install demo dependencies and libraries: `cjson` and `curl`
  3. Have your qispace meta config `.json` file from Quantropi Inc. ready. If you do not have one, please contact Quantropi Inc..

### Running the demo

  1. **Generate key demo:**
      - Execute the `demo_sequr_key_gen` with the below arguments:
        ```
        usage: ./demo_sequr_key_gen [-h] --qispace_meta QISPACE_META [--key_size_bits KEY_SIZE_BITS]

        options:
          -h, --help        Show this help message and exit
          --qispace_meta    Path to qispace meta .json file, provided by Quantropi Inc.
          --key_size_bits   Key size to generate (in bits, defaults: 256)
        ```
      - You might get an output as below:
        ```
        ------------------------
        Key generation successful
        Key ID: 60d27860-3be1-4eae-92a3-2c10129eeb60
        Key: 86 83 b1 57 a2  ...  b7 90 83 65 fe
        ------------------------
        ```
  2. **Query key demo:**
      - Execute the `demo_sequr_key_query` with the below arguments:
        ```
        usage: ./demo_sequr_key_query [-h] [--qispace_meta QISPACE_META] [--key_id KEY_ID]

        options:
          -h, --help           Show this help message and exit
          --qispace_meta       Path to qispace meta .json file, provided by Quantropi Inc.
          --key_id             Key ID to query
        ```
      - You might get an output as below:
        ```
        ------------------------
        Key query successful
        Key: 86 83 b1 57 a2  ...  b7 90 83 65 fe
        ------------------------
        ```
  3. **Compare results**

      Note that for the same `key_id`, we were able to retrieve the same key content.
