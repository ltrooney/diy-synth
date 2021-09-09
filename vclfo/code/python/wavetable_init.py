import numpy as np

# constants
SMPL_SIZE = 256 # number of samples in each wave in the wavetable
NUM_OCTAVES = 10
NUM_WAVES = 4

fname_meta = "wavetable_meta.dat"
fname = "wavetable_init.dat"

def wavetable_init():
    MAX_H = 368

    t = np.linspace(0, 1, SMPL_SIZE, endpoint=False)
    x = 2*np.pi*t
    num_harmonics_per_octave = [int(MAX_H/(2**i)) for i in range(0, NUM_OCTAVES)]
        
    wavetable, sine_samples, tri_samples, saw_samples, square_samples = ([] for _ in range(NUM_WAVES + 1))
    
    for i in range(0, NUM_OCTAVES):
        sine_samples.append(np.sin(x))
        
        tri_offs = saw_offs = square_offs = 0
        w_tri = w_saw = w_square = np.sin(x)
            
        for k in range(2, num_harmonics_per_octave[i]):
            if k % 2 != 0: # only odd harmonics
                # tri
                tri_offs += np.pi
                new_wave_tri = (1/(k*k)) * np.sin(x*k + tri_offs)
                w_tri = [a + b for a, b in zip(w_tri, new_wave_tri)]

                # square
                new_wave_square = (1/k) * np.sin(x*k)
                w_square = [a + b for a, b in zip(w_square, new_wave_square)]

            # saw
            saw_offs += np.pi
            new_wave_saw = (1/k) * np.sin(x*k + saw_offs)
            w_saw = [a + b for a, b in zip(w_saw, new_wave_saw)]


        tri_samples.append(w_tri)
        saw_samples.append(w_saw)
        square_samples.append(w_square)
    
    wavetable.append(sine_samples) # sine
    wavetable.append(tri_samples) # triangle
    wavetable.append(saw_samples) # saw    
    wavetable.append(square_samples) # square
        
    return t, wavetable

_, wavetable = wavetable_init()

wavetable = np.array(wavetable)
F_PRECISION = 6 # number of digits after the decimal place

wt_flat = np.around(wavetable.flatten(), decimals=F_PRECISION)

# write the wavetable meta file
with open(fname_meta, "w") as f:
    f.write(f'{SMPL_SIZE}\n')
    f.write(f'{NUM_OCTAVES}\n')
    f.write(f'{NUM_WAVES}\n')

# write the output file
with open(fname, "w") as f:
    for i, sample in enumerate(wt_flat):
        thing_to_write = str(sample)
        if i < len(wt_flat)-1:
            thing_to_write += ",\n"
        f.write(thing_to_write)
    f.close()