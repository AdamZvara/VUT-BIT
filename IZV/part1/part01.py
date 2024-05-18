#!/usr/bin/env python
"""
IZV cast1 projektu
Autor: Adam Zvara (xzvara01)

Detailni zadani projektu je v samostatnem projektu e-learningu.
Nezapomente na to, ze python soubory maji dane formatovani.

Muzete pouzit libovolnou vestavenou knihovnu a knihovny predstavene na
prednasce
"""


from bs4 import BeautifulSoup
import requests
import numpy as np
import matplotlib.pyplot as plt
from typing import List


def integrate(x: np.array, y: np.array) -> float:
    """
    Numeral integration using trapezoid rule

    :param x: x values of integral
    :param y: y values of integral
    :returns: value of calculated integral
    :raises ValueError: operands are not the same shape
    :raises TypeError: operands are not numpy arrays
    """
    return np.sum((x[1:] - x[:-1]) * ((y[1:] + y[:-1]) / 2))


def generate_graph(a: List[float], show_figure: bool = False, save_path: str |
                   None = None):
    """
    Generate graph of function f(x) = a * (x ^ 2) on interval <-3,-3>

    :param a: list of 'a' coeficients
    :param show_figure: bool representing whether to plot the graph on screen
    :param save_path: path where to save the figure
    """
    if not isinstance(a, list):
        raise TypeError("Argument a is not list type")

    minX = -3
    maxX = 3

    x = np.linspace(minX, maxX, 100)
    a = np.array(a)
    y = np.power(x, 2) * a.reshape(-1, 1)

    fig, ax = plt.subplots(figsize=(7, 4))

    ax.set_xlabel("x")
    ax.set_ylabel("$f_{a}(x)$")
    ax.margins(x=0)
    ax.set_ylim((-20, 20))

    # move spines so annotations are in bounding box
    ax.spines["top"].set_bounds(minX, maxX + 1)
    ax.spines["bottom"].set_bounds(minX, maxX + 1)
    ax.spines["right"].set_position(("data", maxX + 1))

    for i in range(len(a)):
        # curly brackets are escaped by doubling them
        ax.plot(x, y[i], label=r"$y_{{{}}}(x)$".format(a[i]))
        ax.annotate(r"$\int f_{{{}}}(x)dx$".format(a[i]), (x[-1], y[i, -1]-1))
        ax.fill_between(x, y[i], alpha=0.1)

    ax.legend(loc="lower center", ncol=3, bbox_to_anchor=(0.5, 1.0))

    if show_figure:
        # tight layout centeres the plot
        plt.tight_layout()
        plt.show()

    if save_path:
        fig.savefig(save_path, bbox_inches="tight", pad_inches=0.5)


def generate_sinus(show_figure: bool = False, save_path: str | None = None):
    """
    Generate 3 graphs of different sinus functions

    :param show_figure: bool representing whether to plot the graphs on screen
    :param save_path: path where to save the figure
    """
    t = np.linspace(0, 100, 10000)
    f1 = 0.5 * np.sin(np.pi * t / 50)
    f2 = 0.25 * np.sin(np.pi * t)
    f3 = f1 + f2

    fig, axes = plt.subplots(nrows=3, constrained_layout=True, figsize=(6, 8))

    # setting visuals for each graph
    for ax in axes:
        ax.set_ylim((-0.8, 0.8))
        ax.set_yticks([-0.8, -0.4, 0.0, 0.4, 0.8])
        ax.set_xmargin(0)
        ax.set_xlabel("t")

    (ax1, ax2, ax3) = axes

    ax1.set_ylabel("$f_1(x)$")
    ax1.plot(t, f1)

    ax2.set_ylabel("$f_2(x)$")
    ax2.plot(t, f2)

    ax3.set_ylabel("$f_1(x) + f_2(x)$")
    # making bool list of all green and red points on graph
    above = f3 > f1
    f4 = f3.copy()
    # insert NaN values so graphs are not connected
    f3[~above] = np.nan
    f4[above] = np.nan
    ax3.plot(t, f3, c="green")
    ax3.plot(t, f4, c="red")

    if show_figure:
        plt.show()

    if save_path:
        fig.savefig(save_path)


def download_data(url="https://ehw.fit.vutbr.cz/izv/temp.html"):
    """
    Download temperature data from given page and parse it into dictionary

    :param url: URL of webpage
    :returns: list of dictionaries each representing one row from table
    """
    html = requests.get(url)
    soup = BeautifulSoup(html.text, "html.parser")

    dictionaries = []
    for row in soup.find_all("tr"):
        year, month, *temp = row.find_all("p")
        temp = [float(item.contents[0].replace(',', '.')) for item in temp]
        dictionaries.append({"year": int(year.contents[0]),
                             "month": int(month.contents[0]),
                             "temp": np.array(temp)})
    return dictionaries


def get_avg_temp(data, year=None, month=None) -> float:
    """
    Get average temerature from given data
    Data is in the same format as output of download_data function

    :param data: temperature data
    :param year: set specific year to get temperatures from
    :param month: set specific month to get temperatures from
    :returns: float value representing average of temperatures for given time
    """
    # create temp_sums list that contains tuples
    # (summed_temperatures, temperatures_count) for each month
    if year and month:
        temp_sums = [(np.sum(record.get("temp")), np.size(record.get("temp")))
                     for record in data if record.get("year") == year and
                     record.get("month") == month]
    elif year:
        temp_sums = [(np.sum(record.get("temp")), np.size(record.get("temp")))
                     for record in data if record.get("year") == year]
    elif month:
        temp_sums = [(np.sum(record.get("temp")), np.size(record.get("temp")))
                     for record in data if record.get("month") == month]
    else:
        temp_sums = [(np.sum(record.get("temp")), np.size(record.get("temp")))
                     for record in data]
    temps, cnt = tuple(map(sum, zip(*temp_sums)))
    return temps / cnt
