#!/usr/bin/python2.7
"""
    File name: statistics.py
    Author: zeabus
    Date created: 2018/10/16
    Python Version: 2.7
"""
import numpy as np


class Statistic():
    def __init__(self):
        pass

    def convert_to_oneD(self, data):
        if len(data.shape) == 2:
            return data.ravel()
        return data

    def convert_to_np(self, data):
        return np.array(data)

    def get_mode(self, data):
        data = self.convert_to_oneD(data)
        count = np.bincount(data)
        max = count.max()
        count = list(count)
        return count.index(max)

    def get_median(self, data):
        return np.median(data)

    def get_mean(self, data):
        return data.mean()

    def get_std(self, data):
        return data.std()

    def get_max(self, data):
        return data.max()

    def get_min(self, data):
        return data.min()

    def get_range(self, data):
        return self.get_max(data) - self.get_min(data)

    def get_quantile(self, data, q):
        data = self.convert_to_np(data)
        return np.quantile(data, q/4.)

    def get_percentile(self, data, q):
        data = self.convert_to_np(data)
        return np.percentile(data, q)

    def get_skewness(self, data):
        data = self.convert_to_oneD(data)
        min = self.get_min(data)
        max = self.get_max(data)
        std = self.get_std(data)
        mode = self.get_mode(data)
        mean = self.get_mean(data)
        median = self.get_median(data)
        print("MODE:",mode, "MED:",median, "MEAN:",mean, std, max, min, max - min)
        if mean < median < mode:
            return "Negative direction"
        elif mode < median < mean:
            return "Positive direction"
        elif mode == median == mean:
            return "No skew"
        else:
            return "Don't know"


if __name__ == '__main__':
    pass
