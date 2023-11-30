import struct
import ctypes

lib = ctypes.CDLL("shared_memory.so")
msg = ctypes.create_string_buffer(128)
lib.writeToSharedMemory(b'/dev/shm/test.lock', b'/dev/shm/test', b'HelloWorld', 10)
lib.readSharedMemory(b'/dev/shm/test.lock', b'/dev/shm/test', msg, 10)
print(msg.decode())
