"""Descriptive statistics for ordinary integer and floating-point samples."""

from collections import OrderedDict
from math import sqrt


class StatisticsError(ValueError):
    pass


def _require(values, count):
    if len(values) < count:
        raise StatisticsError("not enough data points")
    return values


def mean(data):
    values = _require(list(data), 1)
    total = sum(values)
    length = len(values)
    if isinstance(total, int) and total % length == 0:
        return total // length
    return total / length


def median(data):
    values = _require(sorted(data), 1)
    half = len(values) // 2
    if len(values) % 2:
        return values[half]
    return (values[half - 1] + values[half]) / 2


def median_low(data):
    values = _require(sorted(data), 1)
    return values[(len(values) - 1) // 2]


def median_high(data):
    values = _require(sorted(data), 1)
    return values[len(values) // 2]


def multimode(data):
    counts = OrderedDict()
    for value in data:
        counts[value] = counts.get(value, 0) + 1
    if not counts:
        return []
    largest = max(counts.values())
    return [value for value in counts if counts[value] == largest]


def mode(data):
    return _require(multimode(data), 1)[0]


def _variance(data, center, sample):
    values = _require(list(data), 2 if sample else 1)
    if center is None:
        # Center first to retain small differences between large readings.
        first = values[0]
        values = [value - first for value in values]
        center = mean(values)
    return sum((value - center) ** 2 for value in values) / (len(values) - sample)


def variance(data, xbar=None):
    return _variance(data, xbar, True)


def pvariance(data, mu=None):
    return _variance(data, mu, False)


def stdev(data, xbar=None):
    return sqrt(variance(data, xbar))


def pstdev(data, mu=None):
    return sqrt(pvariance(data, mu))
