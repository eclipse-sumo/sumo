// Image.h: Schnittstelle für die Klasse Image.
//
//////////////////////////////////////////////////////////////////////

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <fx.h>
#include "gui/GUIApplicationWindow.h"
#include "Point.h"
#include "Graph.h"

class Image {

private:

    //Zwei Pointer auf FXImage-Objekte, über die die Bildbearbeitung abläuft
    FXImage *m_img;
    FXImage *m_transimg;

public:

    //Konstruktoren (1.leer / 2. Zwei Pointer auf ein FXImage-Objekt und ein FXAPP-Objekt)
    Image();

    Image(FXImage *img,FXApp *a);

    //Erstellt eine Kopie des FXImage-Objekts
    void Copy(FXImage *source,FXImage *destination);

    // ???
    int CountTransitions(int i, int j);

    //Holt das FXImage-Objekt
    FXImage* GetFXImage();

    /*
    Erzeugt aus dem Ausgangsbild ein Schwarzweißbild,
    in denen die für uns relevanten Farben in schwarz
    und alle anderen Farben in weiß gefärbt werden
    */
    void ExtractStreets();

    /*
    Führt eine Erosion durch, d.h. eine Verdünnung
    der Schwärzen Flächen des Bildes
    */
    void Erode(ConfigDialog* myDialog);

    /*
    Invers zur Erosion, d.h. hier werden die schwarzen
    Flächen verbreitert
    */
    void Dilate(ConfigDialog* myDialog);

    /*
    Führt erst eine Erosion, dann eine Dilatation durch
    */
    void Opening(ConfigDialog* myDialog);

    /*
    Invers zu Opening(Dilatation->Erosion)
    */
    void Closing(ConfigDialog* myDialog);

    /*
    Färbt weiße Pixel schwarz, die mehr als fünf schwarze
    Nachbarpixel haben
    */
    void CloseGaps();

    /*
    Erzeugt ein Skelett, also Pixelketten aus den
    schwarzen Flächen des Ausgangsbildes
    */
    void CreateSkeleton();

    /*
    Minimiert das Skelett, so daß jedes jetzt
    entfernte schwarze Pixel die Kette sprengt
    */
    void RarifySkeleton();

    /*
    Entfernt scharze Elemente(Fragmente), die
    nicht zu Straßen gehören
    */
    void EraseStains(ConfigDialog* myDialog);

    /*
    Erzeugt einen Graphen aus dem minimalen Skelett
    */
    Graph Tracking(Graph gr, ConfigDialog* myDialog);

    /*
    Hilfsmethode für die Grapherzeugung (Tracking)
    */
    Graph Pixel_Counter(int i,int j,int i_pre, int j_pre,int count,Graph gr,Vertex* temp, int value);

    /*
    Malt die Knoten des Graphen ins Bild
    */
    void DrawVArray(Graph gr);

    /*
    Malt die Kanten des Graphen ins Bild
    */
    void DrawEArray(Graph gr);

    /*
    Malt den Graph ins Bild
    */
    void DrawGraph(Graph gr);

    /*
    Erzeugt einen Punkt auf dem Bild
    */
    void CreatePoint(Point p, bool cross);

    /*
    Erzeugt eine Linie auf dem Bild (anhand zweier Punkte)
    */
    void DrawLine(Point p1, Point p2);

    /*
    Erzeugt eine Linie auf dem Bild (anhand der vier Koordinaten zweier Punkte)
    */
    void DrawLine(FXint i,FXint j,FXint k,FXint l);

    /*
    Erzeugt ein leeres Image
    */
    void EmptyImage();

};

#endif
//_IMAGE_H_
