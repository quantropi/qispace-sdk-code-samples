import requests
import base64
import binascii
from qeep import QeepQSC

class SequrUtil:
 
  def __init__(self, qispace_meta):
    self.url = qispace_meta["url"]
    self.device_token = qispace_meta["token"]

    response = requests.post(
      f"{self.url}/sub_key",
      headers={"Authorization": f"Bearer {self.device_token}"},
    ).json()

    self.qeepQSC = QeepQSC(response["key_index"])
    self.qeepQSC.qk_load(
      binascii.unhexlify(response["sub_key"].encode())
    )

  def key_gen(self, key_size_bytes=32):
    if key_size_bytes < 0:
      raise Exception(f"key_size_bytes must be greater than 0")

    response = requests.post(
      f"{self.url}/qk",
      headers={"Authorization": f"Bearer {self.device_token}"},
      json={"key_length": key_size_bytes}
    ).json()

    key_id = response["id"]
    decryptedQk = self.__qeep_decrypt__(
      response["payload"], response["iv"]
    )
    raw_key = decryptedQk[18: len(decryptedQk)-4]

    return (key_id, raw_key)

  def query_key(self, key_id):
    response = requests.get(
      f"{self.url}/qk/{key_id}",
      headers={"Authorization": f"Bearer {self.device_token}"},
    ).json()

    decryptedQk = self.__qeep_decrypt__(
      response["payload"], response["iv"]
    )
    raw_key = decryptedQk[18: len(decryptedQk)-4]

    return (key_id, raw_key)

  def __qeep_decrypt__(self, base64_cipher, iv):
    self.qeepQSC.set_iv(
      base64.b64decode(iv.encode('utf-8'))
    )

    cipher = base64.b64decode(base64_cipher)
    result, decryptedMessage = self.qeepQSC.decrypt(cipher)
    if result != 1:
      raise Exception(f"failed to decrypt message: {result}")

    return decryptedMessage
