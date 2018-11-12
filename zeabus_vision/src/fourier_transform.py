#!/usr/bin/env python
"""
    File name: fourier_transform.py
    Author: skconan
    Date created: 2018/10/16
    Python Version: 2.7
"""

import numpy as np
import cv2 as cv
import pylab

class FourierTransform:
    def __init__(self):
        self.fft = np.fft
        
    def normalize(self, grayscale, min_in, max_out=255):
        grayscale = grayscale.clip(min=min_in)
        return np.uint8((grayscale - grayscale.min())*max_out/(grayscale.max()-grayscale.min()))

    def image_to_fft(self, grayscale):
        return self.fft.fft2(grayscale)

    def fft_to_image(self, image_fft2):
        return self.fft.ifft2(image_fft2)

    def shift_to_center(self, image_fft2):
        return self.fft.fftshift(image_fft2)

    def image_to_fft_center(self, grayscale):
        """
            Use fourier transform of OpenCV because have a data type problem of numpy library
        """
        grayscale_fft = cv.dft(np.float32(grayscale),flags = cv.DFT_COMPLEX_OUTPUT)
        return self.shift_to_center(grayscale_fft.copy())

    def shift_to_corner(self, image_fft2):
        return self.fft.ifftshift(image_fft2)

    def get_magnitude(self, image_fft2):
        return np.abs(image_fft2)

    def get_log_scale(self, image_fft2):
        magnitude = self.get_magnitude(image_fft2)
        logz = np.log(1+magnitude)
        logz = self.normalize(logz, logz.min(), 255)
        return logz

        

    
    def filtering(self, grayscale, fft_filter):
        image_fft2 = self.image_to_fft_center(grayscale)
       
        fshift = image_fft2 * fft_filter
        cv.imshow('fshift',cv.resize(np.uint8(self.get_log_scale(fshift[:,:,0])),(0,0),fx=0.5,fy=0.5))
        # cv.wa
        # cv.waitKey(-1)
        f_ishift = np.fft.ifftshift(fshift)
        img_back = cv.idft(f_ishift)
        result_filter = cv.magnitude(img_back[:,:,0],img_back[:,:,1])
        result_filter = result_filter.clip(min=0)
        result_filter = 255*(result_filter.copy() - result_filter.min()) / (result_filter.max()-result_filter.min())
 
        return np.uint8(result_filter)
