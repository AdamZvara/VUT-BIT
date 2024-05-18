#!/usr/bin/python3.10
# coding=utf-8
# author: Adam Zvara, xzvara01(@stud.fit.vutbr.cz)
# date: 12-2022

import pandas as pd
import geopandas
import matplotlib.pyplot as plt
import contextily
import sklearn.cluster
import numpy as np


def make_geo(df: pd.DataFrame) -> geopandas.GeoDataFrame:
    """
    Convert dataframe to geopandas.GeoDataFrame with right types

    :param df: original DataFrame
    :returns: GeoDataFrame with correct types and geometry
    """
    # zbavenie sa nehod bez miesta
    df2 = df.dropna(subset=["d", "e"]).copy()
    # pretypovanie p2a na datum
    df2["p2a"] = pd.to_datetime(df2["p2a"]).astype("datetime64[ns]")
    # vytvorenie geodataframe
    gdf = geopandas.GeoDataFrame(
        df2,
        geometry=geopandas.points_from_xy(df2["d"], df2["e"]),
        crs="EPSG:5514",
    )
    return gdf


def plot_geo(
    gdf: geopandas.GeoDataFrame,
    fig_location: str = None,
    show_figure: bool = False,
):
    """
    Plot graphs of alcohol involved accidens for years 2018-2021

    :param gdf: GeoDataFrame
    :param fig_location: path where to store plotted graph
    :param show_figure: whether to show graph
    """
    region = "JHM"
    years = range(2018, 2022)
    gdf_c = gdf.copy()
    # Filtracia podla kraju, p11 a rokov
    gdf_c = gdf_c[gdf_c["region"] == region][["p2a", "p11", "geometry"]]
    gdf_c = gdf_c[(gdf_c["p11"] >= 3) & (gdf_c["p2a"].dt.year.isin(years))]
    # Prevedenie na CRS WGS84
    gdf_c = gdf_c.to_crs("epsg:3857")
    # Plotovanie
    fig, ax = plt.subplots(
        2,
        2,
        figsize=(11, 8),
        layout="constrained",
        gridspec_kw={"hspace": 0.1},
    )
    ax = ax.flatten()
    for index, year in enumerate(years):
        gdf_c[gdf_c["p2a"].dt.year == year].plot(
            ax=ax[index], color="tab:red", markersize=5, alpha=0.5
        )
        contextily.add_basemap(
            ax[index], source=contextily.providers.Stamen.TonerLite
        )
        ax[index].axis("off")
        ax[index].title.set_text(f"{region} kraj ({year})")
    fig.suptitle(
        f"Polohy nehod v kraji {region} pro roky {years[0]}-{years[-1]}"
    )
    if show_figure:
        plt.show()
    if fig_location:
        fig.savefig(fig_location, bbox_inches="tight")


def plot_cluster(
    gdf: geopandas.GeoDataFrame,
    fig_location: str = None,
    show_figure: bool = False,
):
    """
    Plot graph of clustered accidents for single region

    :param gdf: GeoDataFrame
    :param fig_location: path where to store plotted graph
    :param show_figure: whether to show graph
    """
    # Filtracia a uprava geometrie
    region = "JHM"
    road_type = range(1, 4)
    gdf_c = gdf.copy()
    gdf_c = gdf_c[gdf_c["region"] == region][["p36", "geometry"]]
    gdf_c = gdf_c[gdf_c["p36"].isin(road_type)]
    gdf_c = gdf_c.to_crs("epsg:3857")
    # Vytvorenie klastru
    coords = np.dstack([gdf_c.geometry.x, gdf_c.geometry.y]).reshape(-1, 2)
    # Ako zhlukovaciu metodu pouzivam Birch, ktora vyuziva euklidovsku
    # vzdialenost medzi bodmi a redukuje clustering okrajovych bodov
    # Okrem toho som skusal MiniBatchKMeans a AgglomerativeClustering
    # Pocet clusterov by nemal byt mensi nez 20, v opacnom pripade
    # je v jednom clusteri prilis mnoho bodov (mozeme si vsimnut ze
    # legenda zacina vysokym cislom) a rozdelenie do vysoko nehodovych
    # usekov nie je jednoznacne. Na druhu stranu ak je pocet vacsi nez 30
    # je problematicke sa v grafe zorientovat a pri este vacsiom pocte
    # si opät pomozem legendou pretoze tam su prilis nizke hodnoty co znaci
    # vysoky pocet clusterov
    db = sklearn.cluster.Birch(n_clusters=24).fit(coords)
    gdf_c["cluster"] = db.labels_
    gdf_c = gdf_c.dissolve(by="cluster", aggfunc={"p36": "count"})
    # Vykreslenie
    fig, ax = plt.subplots(1, 1, figsize=(12, 7), layout="constrained")
    gdf_c.plot(
        ax=ax,
        markersize=4,
        column="p36",
        legend=True,
        legend_kwds={
            "label": "Počet nehod v úseku",
            "orientation": "horizontal",
            "shrink": 0.7,
            "aspect": 25,
        },
    )
    contextily.add_basemap(ax, source=contextily.providers.Stamen.TonerLite)
    ax.axis("off")
    ax.title.set_text(
        f"Nehody v {region} kraji na silnicích 1., 2. a 3. třídy"
    )
    if show_figure:
        plt.show()
    if fig_location:
        fig.savefig(fig_location, bbox_inches="tight")
    return gdf_c


if __name__ == "__main__":
    # zde muzete delat libovolne modifikace
    gdf = make_geo(pd.read_pickle("accidents.pkl.gz"))
    plot_geo(gdf, "test1.png", True)
    plot_cluster(gdf, "test2.png", True)
