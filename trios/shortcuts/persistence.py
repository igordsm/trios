

import pickle
import gzip

def save_operator(op, fname):
    with gzip.open(fname, 'wb') as f:
        pickle.dump(op, f, -1)

def load_operator(fname):
    with gzip.open(fname, 'rb') as f:
        return pickle.load(f)

def load_image(fname):
    return sp.ndimage.imread(fname, mode='L')

def save_image(image, fname):
    sp.misc.imsave(fname, image)