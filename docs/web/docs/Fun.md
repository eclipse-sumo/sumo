---
title: Fun
---

# Interactive Traffic Light Game

SUMO includes an interactive traffic light control game in `tools/game/` where players click junctions in [sumo-gui](sumo-gui.md) to switch signal phases and minimize waiting times and emissions:

```bash
python tools/game/runner.py
```

!!! note
    The 3D game configurations `bs3d` (*3D Junction Virtual World*) and `cross3d` reference external OpenSceneGraph `.ive` environment and vehicle models (such as `bs_141125_small_1/master.ive` and `Trian_ROD_GTA_Landmarks_Baeumen_Lampen/master.ive`) whose third-party licenses do not permit redistribution in SUMO packages. Use `bs3Dosm` (*3D Junction OpenStreetMap*) or the 2D scenarios when those proprietary `.ive` assets are unavailable locally.

# SUMO Song (German)

**Aber bitte mit SUMO**; slightly censored and unfortunately with no
YouTube pendant.

```
 C                                                   F      C
Sie trafen sich täglich um viertel nach Acht, oh ho ho, oh yeah.

                                            F      C
In ... Zimmer es wurde laut gedacht, oh ho ho, oh yeah.

     F                          C
Die Füße auf dem Tisch, den Kaffee schwarz und heiß

     F                          G
Das Hämmern im Kopf: ..., was'n das für'n Scheiß

     C                               F              F F# G
Wir simulieren viel besser, viel schneller und soooo

              C
Und zwar mit SUMO - aber bitte mit SUMO.



 C                                                 F      C
Die Entwicklung begann vor einem Jahrzehnt, oh ho ho, oh yeah.

                                                           F      C
Es war ... Traum, wonach er sich immer gesehnt, oh ho ho, oh yeah.

 F                 C
Fußgänger, Ampeln, Autos und Straßen

     F                  G
Das war seine Welt über alle Maßen

     C                                   F               F F# G
Mit C++ ging's dann ans Werk - manchmal sogar auf dem Klo

              C
Immer wieder SUUUMO - aber bitte mit SUMO



C                                             F      C
Es wurde entwickelt tagaus und tagein, oh ho ho, oh yeah.

                                                       F      C
Kompiliert und debugged, ja, das musste so sein, oh ho ho, oh yeah.

    F                      C
Da waren die Bugs und die feature requests

     F                           G
Die ... fertig machten, ja, das war kein Fest

    C                F                F F# G
Es wurde gehackt, gefixed und getestet

                      C
Version 0.11 war die Beste - aber bitte mit SUMO



 C                                              F      C
... war glücklich, denn er hatte zu tun, oh ho ho, oh yeah

                                                F      C
Immerzu hacken, nie Zeit, um auszuruh'n, oh ho ho, oh yeah

     F                               C
Bei SUMO gibt's kein Diabetis, kein Meckern und Hau'n

      F                          G
Viel besser als Schokolade, Zigaretten und Frau'n

      C                            F               F F# G
... grinste breit, er jubelte und war sehr frooh-ho

              C
Es war wegen SUMO - aber bitte mit SUMO


 C                                              F      C
Das Ende vom Lied hat wohl jeder erahnt, oh ho ho, oh yeah

                                        F      C
Der Tod hat ... und co abgesahnt, oh ho ho, oh yeah

 F                    C
Übrig blieb SUMO als einzige Simulation von allen

 F                 G
Sie lebe hoch dem ... sei Dank

    C                         F           F F# G
Wäre .. nicht gewesen, wäre ... nun krank

                     C
Dann aber bitte mit SUMO - aber bitte mit SUMO.


Noch ein Schnippsel Code - aber bitte mit SUMO
Zum Emissionsmodell dazu - aber bitte mit SUMO
Oder doch Intermodalität - aber bitte mit SUMO
```
