#!/usr/bin/env python3.9
# coding=utf-8
"""
IZV projekt2.

Autor: Adam Zvara (xzvara01)
"""

from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns
import numpy as np
import zipfile
# muzete pridat libovolnou zakladni knihovnu ci knihovnu predstavenou
# na prednaskach, dalsi knihovny pak na dotaz


# Ukol 1: nacteni dat ze ZIP souboru
def load_data(filename: str) -> pd.DataFrame:
    """
    Load data into pandas dataframe.

    Load data from yearly csv files for each region which are zipped
    in root zip file. Add region column to data based on file name.

    :param filename: root zip filename
    :returns: dataframe with loaded data
    """
    # tyto konstanty nemente, pomuzou vam pri nacitani
    headers = ["p1", "p36", "p37", "p2a", "weekday(p2a)", "p2b", "p6", "p7",
               "p8", "p9", "p10", "p11", "p12", "p13a", "p13b", "p13c", "p14",
               "p15", "p16", "p17", "p18", "p19", "p20", "p21", "p22", "p23",
               "p24", "p27", "p28", "p34", "p35", "p39", "p44", "p45a", "p47",
               "p48a", "p49", "p50a", "p50b", "p51", "p52", "p53", "p55a",
               "p57", "p58", "a", "b", "d", "e", "f", "g", "h", "i", "j", "k",
               "l", "n", "o", "p", "q", "r", "s", "t", "p5a"]

    regions = {
        "PHA": "00", "STC": "01", "JHC": "02", "PLK": "03", "ULK": "04",
        "HKK": "05", "JHM": "06", "MSK": "07", "OLK": "14", "ZLK": "15",
        "VYS": "16", "PAK": "17", "LBK": "18", "KVK": "19",
    }

    # Invertovanie slovnika regions
    inv_regions = {v: k for k, v in regions.items()}
    r_keys = list(inv_regions.keys())

    dataframe_list = []
    with zipfile.ZipFile(filename) as root_zip:
        for year_zip in root_zip.filelist:
            with zipfile.ZipFile(root_zip.open(year_zip)) as nf:
                for csv_file in r_keys:
                    # Vytvorenie DF zo suboru
                    data = nf.open(csv_file+".csv")
                    df = pd.read_csv(
                        data,
                        delimiter=';',
                        decimal=",",
                        names=headers,
                        encoding="cp1250",
                        low_memory=False
                    )
                    # Pridanie regionu
                    df["region"] = inv_regions[csv_file]
                    dataframe_list.append(df)
    return pd.concat(dataframe_list)  # spojenie vsetkych DF


# Vypocet velkosti pandas dataframe v MB
def _dataframe_memory_size(df: pd.DataFrame) -> int:
    """Calculate memory usage of dataframe in MB."""
    return df.memory_usage(deep=True).sum()/(10**6)


# Ukol 2: zpracovani dat
def parse_data(df: pd.DataFrame, verbose: bool = False) -> pd.DataFrame:
    """
    Retype data into appropriate types.

    Parse data in pandas dataframe, drop duplicates, convert columns to
    appropriate types.

    :param df: pandas dataframe
    :param vebose: print original and parsed dataframe sizes to standard output
    :returns: parsed dataframe
    """
    # Vyvorenie kopie povodneho dataframe s filtrovanim opakujucich sa p1
    new_df = df.drop_duplicates(subset="p1").copy()
    # Pretypovanie niektorych stlpcov na kategoricky typ
    noncat_cols = ["p1", "p13a", "p13b", "p13c", "p14", "p34", "p37", "p53",
                   "region"]
    cat_cols = [c for c in new_df.columns.to_list() if c not in noncat_cols]
    new_df[cat_cols] = new_df[cat_cols].astype('category')
    # Vytvorenie noveho stlpca date, konvertovanie dat. typu a vymazanie p2a
    new_df["p2a"] = pd.to_datetime(new_df["p2a"]).astype('datetime64[ns]')
    new_df.rename(columns={'p2a': 'date'}, inplace=True)
    # Reprezentacia stlpcov d, e ako desatinne cisla
    for col in ["a", "b", "d", "e", "f", "g"]:
        new_df[col] = pd.to_numeric(new_df[col], errors="coerce")

    if (verbose):
        print(f"orig_size= {_dataframe_memory_size(df):.1f}MB")
        print(f"new_size= {_dataframe_memory_size(new_df):.1f}MB")

    return new_df


def _conv_visibility(value):
    """Convert visibility from categorical type to string representation."""
    d = "ve dne - "
    n = "v noci - "
    b = "zhoršená"
    g = "nezhoršená"
    if value == 1:
        return d+g
    elif value in [2, 3]:
        return d+b
    elif value in [4, 6]:
        return n+g
    else:
        return n+b


# Ukol 3: počty nehod v jednotlivých regionech podle viditelnosti
def plot_visibility(df: pd.DataFrame, fig_location: str = None,
                    show_figure: bool = False):
    """
    Plot graphs.

    Plot graphs of all accidents from 4 czech regions.
    Each graph shows different visibility during the crash
    for each region

    :param df: pandas dataframe
    :param fig_location: path to save figure to
    :param show_figure: whether to draw figure
    """
    df_c = df.copy()
    # Vybratie konkretnych regionov na vykreslenie
    regions = ["PHA", "JHC", "PLK", "STC"]
    df_c = df_c.loc[df_c["region"].isin(regions)]
    # Zmena kategorickych hodnot na textovy retazec
    df_c["p19"] = df_c["p19"].map(_conv_visibility)
    # Vytvorenie pomocneho stlpca a agregovacia funkcia
    df_c["tmp"] = 1
    df_c = df_c.groupby(["region", "p19"])[["tmp"]].agg("sum")
    df_c.reset_index(inplace=True)

    # Vykreslenie grafov
    sns.set_style("darkgrid")  # nastavenie gridu
    g = sns.catplot(
        x="region",
        y="tmp",
        col="p19",
        data=df_c,
        kind="bar",
        col_wrap=2,
        height=3.5,
        aspect=1.2,
        sharey=False,
        sharex=False,
        hue="region",
        dodge=False
    )
    # Nastavenie vzhladu grafu
    g.set(ylabel="Počet nehod", xlabel=None)
    g.set_titles("Viditelnost: {col_name}")
    g.figure.suptitle("Počet nehod dle vidtelnosti")
    # Vypis cisla pre konkretne stlpce
    # zdroj: https://stackoverflow.com/a/67594395
    for ax in g.axes.ravel():
        for c in ax.containers:
            labels = [f'{v.get_height():.0f}' for v in c]
            ax.bar_label(c, labels=labels, label_type='edge')
        ax.margins(y=0.2)
    plt.tight_layout()
    # Ulozenie a vykreslenie grafu
    if (fig_location):
        g.figure.savefig(fig_location)
    if (show_figure):
        plt.show()


# Ukol4: druh srážky jedoucích vozidel
def plot_direction(df: pd.DataFrame, fig_location: str = None,
                   show_figure: bool = False):
    """
    Plot graphs.

    Plot graphs of all accidents from 4 czech regions.
    Each graph shows 3 directions of cars in accidents

    :param df: pandas dataframe
    :param fig_location: path to save figure to
    :param show_figure: whether to draw figure
    """
    df_c = df.copy()
    regions = ["OLK", "PAK", "VYS", "ZLK"]
    df_c = df_c.loc[df_c["region"].isin(regions)]
    df_c = df_c.loc[df_c["p7"] != 0]  # vyfiltrovanie p7 = 0
    # Zmena kategorickych hodnot na textovy retazec
    df_c["cnt"] = 1  # pomocny stlpec
    df_c["p7"] = df_c["p7"].map(
        {1: "čelní", 2: "boční", 3: "boční", 4: "zezadu"})
    df_c["month"] = df_c["date"].dt.month
    df_c = df_c.groupby(["region", "p7", "month"])[["cnt"]].agg("sum")
    df_c.reset_index(inplace=True)
    # Vykreslenie jednotlivych regionov
    sns.set_style("darkgrid")  # nastavenie gridu
    g = sns.catplot(
        x="month",
        y="cnt",
        col="region",
        data=df_c,
        hue="p7",
        kind="bar",
        col_wrap=2,
        height=3.5,
        aspect=1.2,
        sharey=False,
        sharex=False
    )
    g.set(ylabel="Počet nehod", xlabel="Měsíc")
    g.set_titles("Kraj: {col_name}")
    sns.move_legend(g, "center left", bbox_to_anchor=(1.0, .5),
                    title='Druh srážky')
    plt.tight_layout()
    # Ulozenie a vykreslenie grafu
    if (fig_location):
        g.figure.savefig(fig_location, bbox_inches='tight')
    if (show_figure):
        plt.show()


def _conv_severity(row):
    """Convert severity from categorical type to string representation."""
    if row["p13a"]:
        return "Usmrcení"
    elif row["p13b"]:
        return "Těžké zranění"
    elif row["p13c"]:
        return "Lehké zranění"
    else:
        return "None"


# Ukol 5: Následky v čase
def plot_consequences(df: pd.DataFrame, fig_location: str = None,
                      show_figure: bool = False):
    """
    Plot graphs.

    Plot graphs of all accidents from 4 czech regions.
    Each graph shows severity of each accident
    between years: 2016-2022

    :param df: pandas dataframe
    :param fig_location: path to save figure to
    :param show_figure: whether to draw figure
    """
    df_c = df.copy()
    regions = ["OLK", "PAK", "VYS", "ZLK"]
    df_c = df_c.loc[df_c["region"].isin(regions)]
    # Urcenie dosledkov nehody na zaklade p13(a,b,c)
    df_c["Následky"] = df_c.apply(lambda row: _conv_severity(row), axis=1)
    # Vytvorenie multiindexu a agregovanie nehod podla typu do stlpcov
    df_c = pd.pivot_table(
        df_c,
        index=["date", "region"],
        columns="Následky",
        values="p9",
        aggfunc="count"
    )
    df_c.drop(columns=["None"], inplace=True)
    # Prevedie index region na stlpce aby sa nestratili pri resample
    df_c = df_c.unstack()
    # Podvzorkovanie na mesiace z indexu (jediny index je date)
    # a stack, ktory sposoby vytvorenie multiindexu (date + region)
    df_c = df_c.resample("M").sum().stack()
    # Odstranenie indexu a transformacia tabulky na zaklade nasledkov nehody
    df_c.reset_index(inplace=True)
    df_c = df_c.melt(
        ["date", "region"],
        ["Lehké zranění",  "Těžké zranění", "Usmrcení"],
        value_name="Počet nehod"
    )
    # Vytvorenie grafu
    g = sns.relplot(
        data=df_c,
        x="date",
        y="Počet nehod",
        col="region",
        hue="Následky",
        kind="line",
        col_wrap=2,
        facet_kws={'sharex': False}
    )
    g.set_titles("Kraj: {col_name}")
    g.set(xlabel=None, ylim=(0, 160))
    # Ulozenie a vykreslenie grafu
    if (fig_location):
        g.figure.savefig(fig_location, bbox_inches='tight')
    if (show_figure):
        plt.show()


if __name__ == "__main__":
    # zde je ukazka pouziti, tuto cast muzete modifikovat podle libosti
    # skript nebude pri testovani pousten primo, ale budou volany konkreni
    # funkce.
    df = load_data("data/data.zip")
    df2 = parse_data(df, True)

    plot_visibility(df2, "01_test.png", True)
    plot_direction(df2, "02_test.png", True)
    plot_consequences(df2, "03_test.png", True)
