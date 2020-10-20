import tensorflow as tf
gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.5)
#config = tf.ConfigProto(gpu_options=gpu_options)

config = tf.ConfigProto(
                         device_count={'CPU' : 1, 'GPU' : 0},
                         allow_soft_placement=True,
                         log_device_placement=False
                       );

serialized = config.SerializeToString()
c = list(map(hex, serialized))


print("uint8_t config[] = {")
print(c)
print("};")
print("TF_SetConfig(opts, (void*)config, ",len(serialized),", status);};")
#['0x32', '0x9', '0x9', '0x0', '0x0', '0x0', '0x0', '0x0', '0x0', '0xe0', '0x3f']

