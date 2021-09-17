### TODO : REWRITE FILE ###

#     def __init__(self, wt_meta_filename):
#         # import the wavetable settings
#         with open(wt_meta_filename) as wt_meta_file:
#             consume_line = lambda : int(wt_meta_file.readline())
#             smpl_size = consume_line()
#             num_octaves = consume_line()
#             num_waves = consume_line()
#             self.__init__(smpl_size, num_octaves, num_waves)
     
#         #import the wavetable
#        with open(wt_filename) as wt_file:
#            self.wavetable = [float(x) for x in wt_file.read().split(',\n')]
            


import numpy as np

# constants
SMPL_SIZE = 256 # number of samples in each wave in the wavetable
NUM_OCTAVES = 10
NUM_WAVES = 4

# write the wavetable meta file
fname_meta = "wavetable_meta.dat"
with open(fname_meta, "w") as f:
    f.write(f'{SMPL_SIZE}\n')
    f.write(f'{NUM_OCTAVES}\n')
    f.write(f'{NUM_WAVES}\n')
    
wavetable = Wavetable(fname_meta)

wavetable = wavetable.get_wavetable()
F_PRECISION = 6 # number of digits after the decimal place
wt_flat = np.around(wavetable.flatten(), decimals=F_PRECISION)

fname = "wavetable_init.dat"

# write the output file
with open(fname, "w") as f:
    for i, sample in enumerate(wt_flat):
        thing_to_write = str(sample)
        if i < len(wt_flat)-1:
            thing_to_write += ",\n"
        f.write(thing_to_write)
    f.close()