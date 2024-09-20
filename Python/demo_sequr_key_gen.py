#!/usr/bin/env python3

from QiSpaceSdkLib import SequrUtil
from binascii import hexlify
import argparse
import json

parser = argparse.ArgumentParser("demo_sequr_key_gen.py")
parser.add_argument(
  "--qispace_meta",
  required=True,
  dest="qispace_meta",
  help="Path to qispace meta .json file, provided by Quantropi Inc",
  type=str
)
parser.add_argument(
  "--key_size_bits", 
  dest="key_size_bits",
  help="Key size to generate (in bits, defaults: 256)",
  default=256,
  type=int
)
args = parser.parse_args()

#########
# Initialize device
qispace_meta_content = json.load(open(args.qispace_meta))
sequr_util = SequrUtil(qispace_meta_content)
#########

#########
# Generate new key
key_id, raw_key = sequr_util.key_gen(args.key_size_bits / 8)
key_hex_string = hexlify(raw_key, " ").decode('utf-8')
hex_string_length = len(key_hex_string)
print("------------------------")
print(f"Key generation successful")
print(f"Key ID: {key_id}")
if hex_string_length > 10*3:
  print(f"Key: {key_hex_string[0:5*3]} ... {key_hex_string[hex_string_length-5*3:hex_string_length]}")
else:
  print(f"Key: {key_hex_string}")
print("------------------------")
#########
