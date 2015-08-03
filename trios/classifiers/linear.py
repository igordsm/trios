# -*- coding: utf-8 -*-
"""
Created on Fri May 15 12:22:04 2015

@author: igor
"""


import numpy as np
import sklearn
from trios.WOperator import Classifier
from trios import util
import pickle

import inspect

class LinearClassifier(Classifier):
    def __init__(self, coefs, bias,  minimize=False):
        self.coefs = coefs
        self.bias = bias
        self.minimize = minimize
    
    def train(self, dataset, **kw):
        pass
    
    def apply(self, fvector):
        return 255 * (np.dot(fvector, self.coefs) + self.bias >= 0)
        # return np.maximum(np.minimum(np.dot(fvector, self.coefs), 255), 0)

    def write_state(self, obj_dict):
        obj_dict['cls'] = pickle.dumps(self.cls)
        obj_dict['min'] = self.minimize
        
    def set_state(self, obj_dict):
        self.cls = pickle.loads(obj_dict['cls'])
        self.minimize = obj_dict['min']
