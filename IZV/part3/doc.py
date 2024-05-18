# author: Adam Zvara, xzvara01(@stud.fit.vutbr.cz)
# date: 12-2022

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt


def parse_data(df: pd.DataFrame) -> pd.DataFrame:
    """
    Parse dataframe and print basic statistics

    Convert column p2a to datetime, filter out year 2021, accidents
    with non-driver fault and non-lethal accident. Print out number
    of all accidents, driver-fault accidents, lethal accidents.

    :param df: input accidents dataframe
    :returns: modified dataframe
    """
    # Cast p2a to date and remove year 2021 (data only until august)
    df["p2a"] = pd.to_datetime(df["p2a"]).astype("datetime64[ns]")
    df = df.loc[df["p2a"].dt.year != 2021]
    # Amount of all accidents
    accident_count = df.shape[0]
    print(f"celkovy pocet nehod: {accident_count}")
    # Amount of all accidents caused by driver + filter
    df = df[df["p10"].isin([1, 2])]
    driver_count = df.shape[0]
    print(
        f"celkovy pocet sposobenych vodicom: {driver_count}",
        f"({driver_count/accident_count*100:.2f}%)",
    )
    # Filter dataframe based on lethal accients (p13a > 0)
    df = df[(df["p13a"] > 0)]
    deadly_count = df.shape[0]
    print(f"pocet smrtelnych nehod: {deadly_count}")
    print(f"pocet umrti:", df["p13a"].sum())
    return df


def analysis(
    df: pd.DataFrame, categories: dict, col_name: str, test_name: str
) -> None:
    """
    Print simple analysis of given column for given categories

    :param df: parsed accidents dataframe
    :param categories: dictionary meaning "name_of_category" : [values]
    :param col_name: column name in dataframe
    :param test_name: name of test which will be printed out
    """
    print(test_name)
    df_tmp = df.groupby([col_name])[["p1"]].agg("count").reset_index()

    for c in categories.keys():
        category_summed = df_tmp[df_tmp[col_name].isin(categories[c])]["p1"]
        category_summed = category_summed.sum()
        print(
            f"\t{c}: {category_summed}",
            f"({category_summed/df.shape[0]*100:.0f}%)",
        )


def plot_main_causes(
    df: pd.DataFrame, figpath=None, show=False, transparent=True
) -> None:
    """
    Plot causes of lethal accidents

    :param df: parsed accidents dataframe
    :param figpath: path where to store created figure
    :param show: whether to plot figure
    """
    # Groupby cause (p12) and temporary column for agg funcion (p1)
    df_c = df.groupby(["p12"])[["p1"]].agg("count").reset_index()
    # Create intervals based on p12 values
    df_c["range"] = pd.cut(df_c["p12"], bins=[200, 300, 400, 500, 600])
    # Groupby causes and calculate sum of all accidents for given cause
    df_c = df_c.groupby(df_c["range"])[["p1"]].sum().reset_index()
    # Plot settings
    p = sns.dark_palette("#69d", reverse=True)
    g = sns.catplot(data=df_c, x="range", y="p1", kind="bar", palette=p)
    g.set_axis_labels("", "Počet nehod")
    # Change ticks from intervals to cause names
    g.set_xticklabels(
        [
            "Neprimeraná\nrýchlosť\njazdy",
            "Nesprávne\npredbiehanie",
            "Nedanie\nprednosti\nv jazde",
            "Nesprávny\nspôsob\njazdy",
        ]
    )
    g.fig.subplots_adjust(top=0.9)
    # g.fig.suptitle("Hlavné príčiny smrteľných nehôd")
    if figpath:
        g.fig.savefig(figpath, transparent=transparent)
    if show:
        plt.show()


# Analysis of lethal accidents based on main causes for each region
def region_table(df: pd.DataFrame) -> None:
    """
    Print table containing number of accidents for each region
    and each cause of lethal crash

    :param df: parsed accidents dataframe
    """
    df_c = df.copy()
    df_c = df_c.groupby(["region", "p12"])[["p1"]].count().reset_index()
    df_c["range"] = pd.cut(
        df_c["p12"],
        bins=[200, 300, 400, 500, 600],
        labels=["rýchlosť", "predbiehanie", "prednost", "spôsob"],
    )
    df_c = df_c.groupby(["region", "range"])[["p1"]].sum().reset_index(level=1)
    df_c = pd.pivot_table(
        df_c, values="p1", index=df_c.index, columns="range", aggfunc="first"
    )
    df_c.columns.name = None
    print(df_c)


if __name__ == "__main__":
    # Load dataframe
    df = pd.read_pickle("accidents.pkl.gz")
    # Parse data
    print(7 * "-" + " zakladne informacie " + 7 * "-")
    df_2 = parse_data(df)
    # Print out different analyses
    print(+7 * "-" + " analyzy nehod " + 7 * "-")
    # analysis of substance use
    categories = {
        "alkohol": [1, 3, 6, 7, 8, 9],
        "drogy": [4],
        "oboje": [5],
        "ziadne": [2],
        "nezistene": [0],
    }
    analysis(df_2, categories, "p11", "alkohol v krvi vodica")
    # analysis of daytime
    categories = {"den": range(1, 4), "noc": range(4, 8)}
    analysis(df_2, categories, "p19", "den vs noc")
    # analysis of visibility
    categories = {"dobre": [1], "noc": [0, 2, 3, 4, 5], "zakryty": [6]}
    analysis(df_2, categories, "p20", "viditelnost")
    # analysis of weather
    categories = {
        "dobre": [1],
        "mlha": [2],
        "dazd": [3, 4],
        "sneh": [5],
        "namraza": [6],
        "naraz. vietor": [7],
    }
    analysis(df_2, categories, "p18", "poveternostne podmienky")
    # Plot causes of lethal accidents
    plot_main_causes(df_2, "fig.png")
    # Print out table
    print(
        7 * "-" + " vypis tabulky krajov a hlavnych pricin nehody " + 7 * "-"
    )
    region_table(df_2)
